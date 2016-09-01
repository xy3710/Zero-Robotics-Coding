//Begin page main
float rotateRate[3];
int counter;

void init(){
	rotateRate[0]=.5;
	rotateRate[1]=.5;
	rotateRate[2]=.5;
	counter = 0;
}

void loop(){
	if(counter<30){
	    api.setAttRateTarget(rotateRate);
	    counter++;
	}
}

//End page main
