#include "gfw.h"

int main(){
	Init(600,800,false);
	DrawableTest omg;
	SetActiveShader(DefaultShader);
	while(true){
		Draw(0,0,&omg);
		Refresh();
	}

	return 0;
}
