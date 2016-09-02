//Begin page main
float stateA[12];
float stateB[12];
float target[3];

void init(){
    
}

void loop(){
    if (api.getTime() == 0) {
	   api.getMyZRState(stateA);
	   api.getOtherZRState(stateB);
    }
    for (int a = 0; a < 3; a++) {
        target[a] = (stateA[a] + stateB[a]) / 2;
    }
    api.setPositionTarget(target);
    
    DEBUG(("Lmao hi justin"));
}

//End page main
