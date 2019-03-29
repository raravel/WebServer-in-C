//download.c
#include "mongoose.h" //헤더파일 포함
#include <stdlib.h>
#include <string.h> 

struct mg_serve_http_opts s_http_server_opts;

//파일의 크기를 구함
int get_file_size(char *path)
{
    FILE *fp;
    int len = 0;
    
    if ( path && (fp = fopen(path, "r")) ) { 
        fseek(fp, 0, SEEK_END);
        len = ftell(fp);
        fclose(fp);
    }   
    return len;
}

#define DOWNLOAD_TYPE_TXT		"text/plain"
#define DOWNLOAD_TYPE_HTML		"text/html"
#define DOWNLOAD_TYPE_STREAM	"application/octet-stream"
#define DOWNLOAD_TYPE_ZIP		"application/zip"
#define DOWNLOAD_TYPE_PDF		"application/pdf"
#define DOWNLOAD_TYPE_DOC		"application/msword"
#define DOWNLOAD_TYPE_EXCEL		"application/vnd.ms-excel"
#define DOWNLOAD_TYPE_PPT		"application/vnd.ms-powerpoint"
#define DOWNLOAD_TYPE_GIF		"image/gif"
#define DOWNLOAD_TYPE_PNG		"image/png"
#define DOWNLOAD_TYPE_JPEG		"image/jpg"

//stringArray에 str이 있는 지 검사한다.
bool stringArrayIndexOf(char *str, char **array, int arrsize)
{
	int i;
	for (i = 0;i < arrsize;i++) {
		if ( strcmp(str, array[i]) == 0) {
			return true;
		}
	}
	return false;
}

//파일의 헤더 타입을 가져온다.
char *get_file_type(char *ext)
{
	char *textList[] = { "txt" };
	char *htmlList[] = { "html", "htm", "shtml" };
	char *streamList[] = { "exe", "bin", "" };
	char *zipList[] = { "zip", "tar.gz" };
	char *pdfList[] = { "pdf" };
	char *docList[] = { "doc", "docx" };
	char *excelList[] = { "xls", "xlsx" };
	char *pptList[] = { "ppt" };
	char *gifList[] = { "gif" };
	char *pngList[] = { "png" };
	char *jpegList[] = { "jpg", "jpeg" };

	if ( stringArrayIndexOf(ext, textList, ARRAY_SIZE(textList)) ) {
		return DOWNLOAD_TYPE_TXT;
	} else if ( stringArrayIndexOf(ext, htmlList, ARRAY_SIZE(htmlList)) ) {
		return DOWNLOAD_TYPE_HTML;
	} else if ( stringArrayIndexOf(ext, streamList, ARRAY_SIZE(streamList)) ) {
		return DOWNLOAD_TYPE_STREAM;
	} else if ( stringArrayIndexOf(ext, zipList, ARRAY_SIZE(zipList)) ) {
		return DOWNLOAD_TYPE_ZIP;
	} else if ( stringArrayIndexOf(ext, pdfList, ARRAY_SIZE(pdfList)) ) {
		return DOWNLOAD_TYPE_PDF;
	} else if ( stringArrayIndexOf(ext, docList, ARRAY_SIZE(docList)) ) {
		return DOWNLOAD_TYPE_DOC;
	} else if ( stringArrayIndexOf(ext, excelList, ARRAY_SIZE(excelList)) ) {
		return DOWNLOAD_TYPE_EXCEL;
	} else if ( stringArrayIndexOf(ext, pptList, ARRAY_SIZE(pptList)) ) {
		return DOWNLOAD_TYPE_PPT;
	} else if ( stringArrayIndexOf(ext, gifList, ARRAY_SIZE(gifList)) ) {
		return DOWNLOAD_TYPE_GIF;
	} else if ( stringArrayIndexOf(ext, pngList, ARRAY_SIZE(pngList)) ) {
		return DOWNLOAD_TYPE_PNG;
	} else if ( stringArrayIndexOf(ext, jpegList, ARRAY_SIZE(jpegList)) ) {
		return DOWNLOAD_TYPE_JPEG;
	}

	//어떤 타입에도 해당하지 않으면 stream으로 보낸다.
	return DOWNLOAD_TYPE_STREAM;
}

//파일 확장자를 가져온다.
//ex) "test.txt" -> "txt"
char *get_file_ext(char *path, char *buf, int buflen)
{
	int pathlen, i, tmpIdx;
	char *tmp;
	bool findDot = false;

	if ( !path || !buf ) return NULL;
	
	tmp = (char*)malloc(sizeof(char) * buflen);
	if ( !tmp ) return NULL;

	pathlen = strlen(path);

	//문자열의 끝부터 . 이 나올 때 까지 배열에 담는다.
	//ex) "test.html" -> "lmth"
	for (i=pathlen-1, tmpIdx=0;i > 0 && i < buflen;i--) {
		if ( path[i] == '.' ) {
			tmp[tmpIdx] = '\0';
			findDot = true;
			break;
		}
		tmp[tmpIdx++] = path[i];
	}

	i = 0;
	// '.' 을 찾았을 때만.
	if ( findDot ) {
		//임시 배열에 담긴 값을 buf에 reverse하여 담는다.
		//현재 tmp[tmpIdx] 는 '\0'이다. 
		for (tmpIdx -= 1;tmpIdx >= 0;tmpIdx--) {
			buf[i++] = tmp[tmpIdx];
		}
	}
	buf[i] = '\0'; //문자열 마지막 NULL 지정
	
	free(tmp);
	tmp = NULL;
	return buf;
}

//파일 다운로드
void download_file(struct mg_connection *nc, char *path, char *name)
{
    char *header = "HTTP/1.1 200 OK\r\n"
        "Content-Type: \"%s\"\r\n"
        "Content-Length: %d\r\n"
        "Content-disposition: attachment; filename=\"%s\"\r\n"
        "Connection: close\r\n\n", *body = NULL, *type = NULL;
    int size = 0;
    FILE *fp;
	char extbuf[32];

    if ( !nc || !path || !name ) return;

	//파일 타입을 가져옴
	type = get_file_type(get_file_ext(path, extbuf, sizeof(extbuf)));

	//파일 데이터를 읽어들임
    size = get_file_size(path);
    if ( (fp = fopen(path, "r")) == NULL ) { 
		perror("File Open Fail\n");
        return;
    }   
    body = (char*)malloc(sizeof(char) * size);
	fread(body, size, 1, fp);
	fclose(fp);

    mg_printf(nc, header, type, size, name); //헤더 전송
	{
		//다운로드할 파일의 데이터 전송
		int i;
		for (i=0;i<size;i++) {
			mg_printf(nc, "%c", body[i]);
		}
	}

    free(body);
    body = NULL;
}


//연결요청이 되었을 때 이벤트 함수
void event_handler(struct mg_connection *nc, int event, void *p)
{
    switch ( event ) {
		//연결요청 이벤트
        case MG_EV_HTTP_REQUEST: {
            //이 이벤트에서 p는 struct http_message * 형이다.
            struct http_message *hm = (struct http_message *) p; 
			if ( mg_vcmp(&hm->method, "POST") == 0 ) {
				//파일 다운로드
				download_file(nc, "down.tmp", "load.tmp");
			} else {
				//GET 요청일 경우
				//요청 정보를 분석해서 해당 경로의 웹 파일을 보여줌.
				mg_serve_http(nc, (struct http_message *) p, s_http_server_opts);
			}
        } break;
    }
}

int main ( void )
{
    struct mg_mgr mgr;
    struct mg_connection *nc;

    mg_mgr_init(&mgr, NULL);
    nc = mg_bind(&mgr, "8000", event_handler); //8000포트 바인딩, 이벤트 함수 지정
    mg_set_protocol_http_websocket(nc); //연결요청 시 들어오는 데이터 분석후 이벤트 함수 호출

	// http://domain.url/ 의 root 경로를 지정.
	// 현재 경로의 web_src 폴더로 지정
	s_http_server_opts.document_root = "./web_src"; 

    while ( 1 ) {
        mg_mgr_poll(&mgr, 1000); //연결요청 대기
    }
    mg_gmr_free(&mgr);

    return 0;
}
