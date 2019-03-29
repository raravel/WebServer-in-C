//document_root.c
#include "mongoose.h" //헤더파일 포함

struct mg_serve_http_opts s_http_server_opts;

//연결요청이 되었을 때 이벤트 함수
void event_handler(struct mg_connection *nc, int event, void *p)
{
    switch ( event ) {
		//연결요청 이벤트
        case MG_EV_HTTP_REQUEST: {
			//요청 정보를 분석해서 해당 경로의 웹 파일을 보여줌.
			mg_serve_http(nc, (struct http_message *) p, s_http_server_opts);
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
