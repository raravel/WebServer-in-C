//SSI.c
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
			} else {
				//GET 요청일 경우
				//요청 정보를 분석해서 해당 경로의 웹 파일을 보여줌.
				mg_serve_http(nc, (struct http_message *) p, s_http_server_opts);
			}
        } break;
		case MG_EV_SSI_CALL: {
			//shtml 에서 ssi로 call 호출
			//struct mg_ssi_call_ctx *pssi = (struct mg_ssi_call_ctx*)p;
			
			const char *param = (const char *) p;

		if (strcmp(param, "ssi_call") == 0) {
				mg_printf(nc, "Hello From ssi_call\n");
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
