//post_method.c
#include "mongoose.h" //헤더파일 포함
#include <stdlib.h>
#include <string.h> 

struct mg_serve_http_opts s_http_server_opts;

//연결요청이 되었을 때 이벤트 함수
void event_handler(struct mg_connection *nc, int event, void *p)
{
    switch ( event ) {
		//연결요청 이벤트
        case MG_EV_HTTP_REQUEST: {
            //이 이벤트에서 p는 struct http_message * 형이다.
            struct http_message *hm = (struct http_message *) p; 
			if ( mg_vcmp(&hm->method, "POST") == 0 ) {
				//POST 요청일 경우
				char str_value1[BUFSIZ], str_value2[BUFSIZ];

				//hm->body에 post전송 데이터 값이 존재한다.
				//거기서 값을 가져와 담는다.
				mg_get_http_var(&hm->body, "value1", str_value1, sizeof(str_value1));
				mg_get_http_var(&hm->body, "value2", str_value2, sizeof(str_value2));

				//http header를 보낸다. 응답코드 200(정상) Transfer-Encoding: chunked 
				mg_send_head(nc, 200, hm->message.len, "Transfer-Encoding: chunked");

				//결과값을 전송한다.
				mg_printf_http_chunk(nc, "%d", atoi(str_value1) + atoi(str_value2));
				mg_send_http_chunk(nc, "", 0); // 전송의 끝이라는 것을 알리기 위해 빈 값을 전송한다.
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
