#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>


#define HTTP_HEADER_LEN 256
#define HTTP_REQUEST_LEN 256
#define HTTP_METHOD_LEN 6
#define HTTP_URI_LEN 100

#define HTTP_URI_PARAM_LEN 100


#define FILE_NAME_LEN 1000
#define LOG_ENTRY_LEN 1000


#define REQ_END 100
#define ERR_NO_URI -100
#define ERR_ENDLESS_URI -101

char const *index_file = "index.html";

struct http_req {
	char request[HTTP_REQUEST_LEN];
	char method[HTTP_METHOD_LEN];
	char uri[HTTP_URI_LEN];
	char uri_path[HTTP_URI_LEN];
	char uri_params[HTTP_URI_PARAM_LEN];
	// uri_params
	// version
	// user_agent
	// server
	// accept
};

int fill_req(char *buf, struct http_req *req) {
<<<<<<< HEAD
	if ((strlen(buf) == 2) && (buf[0] == '\r') && (buf[1] == '\n')) {
=======
	fprintf(stderr,"REQ line: %s\n",buf);
	if (strlen(buf) == 2) {
>>>>>>> 95f0ec382d159f048ad636e411f0fb883ab4adac
		// пустая строка (\r\n) означает конец запроса
		return REQ_END;
	}
	char *p, *a, *b;
	// Это строка GET-запроса
	p = strstr(buf, "GET");
	if (p == buf) {
		// Строка запроса должна быть вида
		// GET /dir/ HTTP/1.0
		// GET /dir HTTP/1.1
		// GET /test123?r=123 HTTP/1.1
		// и т.п.
		strncpy(req->request, buf, strlen(buf));
		fprintf(stderr, "GET REQ: %s\n",req->request);
		strncpy(req->method, "GET", strlen("GET"));
		a = strchr(buf, '/');
		if ( a != NULL) { // есть запрашиваемый URI 
			b = strchr(a, '?');
			if ( b != NULL ) { // конец URI
			strncpy(req->uri, a, b-a);
				// пусть пока URI_PATH - то же, что и URI
				strncpy(req->uri_path, a, b-a);
			} else {
				b = strchr(a, ' ');
				if (b != 0) {
					strncpy(req->uri, a, b-a);
				}  else {
					return ERR_ENDLESS_URI;  
					// тогда это что-то не то
				}
		}		
		}   else {
                        return ERR_NO_URI; 
                        // тогда это что-то не то
                }

	}
	return 0;	
}

int log_req(char* log_path, struct http_req *req) {
	int fd;
	char log_entry[LOG_ENTRY_LEN] = "Request: ";
	if ((fd = open(log_path, O_WRONLY | O_CREAT | O_APPEND, 0600)) < 0) {
		perror(log_path);
		return 1;
	}
	strcat(log_entry, req->request);
	if (write(fd, log_entry, strlen(log_entry)) != strlen(log_entry)) {
		perror(log_path);
		return 1;
	}
        write(fd, "\n", 1);
        fsync(fd);
        close(fd);
        return 0;
}


/*int make_resp(struct http_req *req) {
	printf("HTTP/1.1 200 OK\r\n");
	printf("Content-Type: text/html\r\n");
	printf("\r\n");
	printf("<html><body><title>Page title</title><h1>Page Header</h><h2>Method: %s</h2><h2>URI: %s</h2><h2><h2>URI Path: %s</h2>URI Parameters: %s</h2></body></html>\r\n", req->method, req->uri, req->uri_path, req->uri_params);
	printf("<html><body><title>Page title</title><h1>Page Header</h1>");
	printf("<p>Method: %s</p>",req->method);
	printf("</body></html>\r\n");*/
int make_resp(char *base_path, struct http_req *req) {
        int fdin;
        struct stat statbuf;
        void *mmf_ptr;
	// определяем на основе запроса, что за файл открыть
	char res_file[FILE_NAME_LEN] = "";
	if (base_path != NULL) {
		strncpy(res_file,base_path,strlen(base_path));
	}
	strcat(res_file,index_file); // вот сюда писать отображение запроса в файловые пути 
	// открываем
        if ((fdin=open(res_file, O_RDONLY)) < 0) {
                perror(res_file);
                return 1;
        }
	// размер
        if (fstat(fdin, &statbuf) < 0) {
                perror(res_file);
                return 1;
        }
	// mmf
        if ((mmf_ptr = mmap(0, statbuf.st_size, PROT_READ, MAP_SHARED, fdin, 0)) == MAP_FAILED) {
                perror("myfile");
                return 1;
        }
	// Выводим HTTP-заголовки
	char *http_result = "HTTP/1.1 200 OK\r\n";
	write(1,http_result,strlen(http_result));
	char *http_contype = "Content-Type: text/html\r\n";
	write(1,http_contype,strlen(http_contype));
	char *header_end = "\r\n";
	write(1,header_end,strlen(header_end));
	//printf("HTTP/1.1 200 OK\r\n");
	//printf("Content-Type: text/html\r\n");
	//printf("\r\n");
	// Выводим запрошенный ресурс
        if (write(1,mmf_ptr,statbuf.st_size) != statbuf.st_size) {
                perror("stdout");
                return 1;
        }
	// Подчищаем ресурсы
        close(fdin);
        munmap(mmf_ptr,statbuf.st_size);
	return 0;
}

int main (int argc, char* argv[]) {
	// первый параметр - каталог с контентом
	// второй параметр - каталог для ведения журнала
	char base_path[FILE_NAME_LEN] = "";
	char log_path[FILE_NAME_LEN] = "";
	char const *log_file = "access.log";
	if ( argc > 2 ) { // задан каталог журнализации
		strncpy(base_path, argv[1], strlen(argv[1]));
		strncpy(log_path, argv[2], strlen(argv[2]));
		strcat(log_path,"/");
		strcat(base_path,"/");
	}
	strcat(log_path,log_file);
	char buf[HTTP_HEADER_LEN];
	struct http_req req;
	while(fgets(buf, sizeof(buf),stdin)) {
		int ret = fill_req(buf, &req);
		if (ret == 0) 
			// строка запроса обработана, переходим к следующей
			continue;
		if (ret == REQ_END ) 
			// конец HTTP запроса, вываливаемся на обработку
			break;
		else
			// какая-то ошибка 
			printf("Error: %d\n", ret);
		
	}
	log_req(log_path, &req);
	make_resp(base_path,&req);
}
