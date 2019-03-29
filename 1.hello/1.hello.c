//hello.c
#include "mongoose.h" //헤더파일 포함

//연결요청이 되었을 때 이벤트 함수
void event_handler(struct mg_connection *nc, int event, void *p)
{
    switch ( event ) {
        case MG_EV_HTTP_REQUEST: {
            //연결요청 이벤트
            //이 이벤트에서 p는 struct http_message * 형이다.
            struct http_message *hm = (struct http_message *) p; 

            //http header를 보낸다. 응답코드 200(정상) Content-Type: text/html
            mg_send_head(nc, 200, hm->message.len, "Content-Type: text/html");

            //페이지 내용을 출력한다.
            //mg_printf(nc, "Hello World!");
			mg_printf(nc, "<button onClick='alert(\"%s\")'>alert</button>\r\n\r\n", "Hello World!");
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

    while ( 1 ) {
        mg_mgr_poll(&mgr, 1000); //연결요청 대기
    }
    mg_gmr_free(&mgr);

    return 0;
}
