// 8.jansson.c
#include <stdio.h>
#include <jansson.h> //jansson 헤더를 추가

int main ( void )
{
	json_t *obj = json_object(); //오브젝트 형식의 json_t 구조체 포인터를 선언. 자바스크립트로 new Object() 라고 생각하면 된다.
	json_object_set(obj, "hello", json_string("hello")); //hello란 키워드에 "hello" 스트링을 설정
	// { hello: "hello" } 가 된 형식.

	json_t *gobj = json_object_get(obj, "hello"); //object타입인 obj 에서 hello 키워드의 값을 가져옮
	printf("%s\n", json_string_value(gobj)); //가져온 값을 string 형으로 가져옮. 반환 형식은 const char*
	
	json_decref(obj); //생성한 json을 free해줌
}
