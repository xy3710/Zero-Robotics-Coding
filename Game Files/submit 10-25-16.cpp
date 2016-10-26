//Begin page main
float myState[12], otherState[12];
float myPos[3], *dest, SPSpos1[3], SPSpos2[3], SPSpos3[3];
float myVel[3], otherPos[3];
float myZone[4],otherZone[3];
float spsDrop1[3],spsDrop2[3],spsDrop3[3];
float enemyItemDistP, enemyItemDist,enemyItemDistP1, enemyItemDist1;
float oldPos[3],eDirVec[3], eZoneVec[3];
bool enemyStealing;
int blue;
int stealCount;
bool    newPath;              
float   forces[3];      
float   targetVec[3];
bool initial;
bool dropSPS;
int itemId;
int time;

//SMDDDD
void init(){
    stealCount = 0;
    time = 0;
	    SPSpos1[0] = 0.0;
	    SPSpos1[1] = 0.15;
	    SPSpos1[2] = 0.0;
	    
	    SPSpos2[0] = -0.3;
	    SPSpos2[1] = .55; 
	    SPSpos2[2] = 0.2;
	    
	    SPSpos3[0] = .3;
	    SPSpos3[1] = .3;
	    SPSpos3[2] = .4;
	    
	    
	    api.getMyZRState(state);
        for(int i =0;i<3;i++){
            currentPos[i]=state[i];
            currentVel[i]=state[i+3];
            currentAtt[i]=state[i+6];
        }
        blue = 0;
        if(currentPos[1] < 0) {
            blue = 1;
            for(int i=0; i<3; i++) {
                SPSpos1[i] *=-1;
                SPSpos2[i] *= -1;
                SPSpos3[i] *= -1;
            }
        }
        itemId = 0;
        initialize = true;
        fminitialize = true;
        fmdistance = 0;
        docked =false;  
        docked1 = false;
        dropped = false;
        dropped1 = false;
        dropSPS = true;
        enemyStealing = false;
        game.dropSPS();
}

void loop() {
    api.getMyZRState(state);
    api.getOtherZRState(otherState);
    for(int i =0;i<3;i++){
        currentPos[i]=state[i];
        currentVel[i]=state[i+3];
        currentAtt[i]=state[i+6];
        otherPos[i] = otherState[i]; 
        otherZone[i] = -1*myZone[i];
    }
	if(enemyStealing){
	    if(docked){
	        if(game.getNumSPSHeld()==2){
	            if(fastMovement(SPSpos3, 0.6, 0.1)) {
                    game.dropSPS();
                    memcpy(spsDrop2,currentPos,sizeof(spsDrop2));
	            }
	        }else{
	            if(game.getNumSPSHeld()==1){
	                if(fastMovement(SPSpos2, 0.6, 0.1)){
	                    game.dropSPS();
	                    game.getZone(myZone);
	                }
	            }else{
                    if(!dropped){
                        if(dockItem(itemId,true)){
                            dropped = true;
                            itemId = 1-itemId;
                            enemyStealing = false;
                        }
                    }
	            }
	        }
        }else{
            if(dockItem(itemId,false)){
                docked = true;
            }
        }
	}else{
	    float distVec[3];
        enemyItemDistP = enemyItemDist;
        enemyItemDistP1 = enemyItemDist1;
        game.getItemLoc(itemPos,itemId);
        enemyItemDist1 = distancecalc(otherPos,itemPos);
        mathVecSubtract(distVec,otherPos,itemPos,3);
        enemyItemDist = distancecalc(otherPos,itemPos);
        if(((enemyItemDist-enemyItemDistP)>(enemyItemDist1-enemyItemDistP1))&&(distancecalc(currentPos,itemPos)-distancecalc(otherPos,itemPos))<.2){
            stealCount++;
        }else{
            stealCount = 0;
        }
    	if(game.getNumSPSHeld() == 2) {
    	    itemId = blue;
            if(fastMovement(SPSpos2, 0.6, 0.15)) {
                //DEBUG(("Dropped SPS2"));
                game.dropSPS();   
                memcpy(spsDrop1,currentPos,sizeof(spsDrop1));
            } else if(time > 30) {
                game.dropSPS();
                memcpy(spsDrop1,currentPos,sizeof(spsDrop1));
            }
            if((stealCount==5)){
                enemyStealing = true;
            }
        }
        if(game.getNumSPSHeld() == 1) {
            if(fastMovement(SPSpos3, 0.6, 0.15)) {
                //DEBUG(("Dropped SPS3"));
                game.dropSPS();
                itemId = blue;
                if(itemId == 6) {
                    dropped = true;
                    docked = true;
                }
            } else if(time > 60) {
                game.dropSPS();
                itemId = blue;
                if(itemId == 6) {
                    dropped = true;
                    docked = true;
                }
            }
            if(stealCount==5){
                enemyStealing = true;
                memcpy(SPSpos2,SPSpos3,sizeof(SPSpos2));
            }
        }
        if(game.getNumSPSHeld() == 0) {
            //DEBUG(("Finished dropping SPS."));
            if(docked){
                game.getZone(myZone);
                if(dropped){
                    if(enemyStealing){
                        api.setPositionTarget(myZone);
                        DEBUG(("-.-"));
                    }else{
                        if(docked1){
                            if(dropped1){
                                if(distancecalc(myZone,otherPos)>.75){
                                    api.setPositionTarget(otherPos);
                                }else{
                                    api.setPositionTarget(myZone);
                                }
                            }
                            else{
                                if(dockItem(itemId,true)){
                                    dropped1 = true;
                                }   
                            }
                        }else{
                            if(dockItem(itemId,false)){
                                DEBUG(("Docked"));
                                docked1 = true;
                            }
                        }
                    }
                    mathVecSubtract(eDirVec,otherPos,oldPos,3);
                    mathVecSubtract(eZoneVec,myZone,otherPos,3);
                    mathVecNormalize(eDirVec,3);
                    mathVecNormalize(eZoneVec,3);
                    memcpy(oldPos,otherPos,sizeof(oldPos));
                    if((acosf(mathVecInner(dirVec,eZoneVec,3))<.5)||distancecalc(otherPos,myZone)<.325){
                        stealCount++;
                        DEBUG(("MEMEMEMEM"));
                    }else{
                        stealCount=0;
                        enemyStealing = false;
                    }
                    if((stealCount>4)&&(distancecalc(currentPos,myZone)-distancecalc(otherPos,myZone))<.2){
                        enemyStealing = true;
                    }
                }else{
                    if(dockItem(itemId,true)){
                        dropped = true;
                        itemId = optimumItem();
                        stealCount =0;
                    }
                }
            }else{
                if(dockItem(itemId,false)){
                    docked = true;
                }
            }
        }
	}
    time++;
}

bool fminitialize,initialize; //tells method if initializen is required for first loop through
float firstPos[3],maxDist,minDist,dist,drop,currentVel[3],currentAtt[3],currentPos[3],loadDest[3],state[12],actualDest[3],dirVec[3],pointVec[3]; //required variables for run
float itemPos[3]; //item position
bool reached,pointing; //boolean is true if item dock position is reached
bool docked,docked1,dropped,dropped1;
float fmdistance;

bool dockItem(int id,bool isZone){
    if(isZone){
        memcpy(itemPos,myZone,sizeof(itemPos));
    }else{
        game.getItemLoc(itemPos,id);
    }
    if(initialize){
        if(id<2){
            maxDist = .173;
            minDist = .151;
        }else if(id<4){
            maxDist = .160;
            minDist = .138;
        }else{
            maxDist = .146;
            minDist = .124;
        }
        if(isZone){
            maxDist = .175;
            minDist = .075;
        }
        initialize = false;
    }
    reached=false;
    pointing = false;
    memcpy(loadDest,itemPos,sizeof(loadDest));
    memcpy(pointVec,currentAtt,sizeof(pointVec));
	mathVecSubtract(dirVec,loadDest,currentPos,3);
	dist = mathVecMagnitude(dirVec,3);
	//DEBUG(("%f",id));
	mathVecNormalize(dirVec,3);
	for(int i=0;i<3;i++){
	    actualDest[i]=dirVec[i]*(minDist+.01);
	}
	mathVecSubtract(actualDest,loadDest,actualDest,3);
	if(dist>minDist && dist<maxDist){
	    reached = true;
	    //DEBUG(("reached"));
	}
	if(acosf(mathVecInner(dirVec,pointVec,3))<.2){
	    pointing = true;
	    //DEBUG(("pointing"));
	}
	if(reached&&pointing){
        if(isZone){
            game.dropItem();
            initialize = true;
	        return true;
        }else{
	        if(game.hasItem(id)!=2){
	            if(game.dockItem(id)){
    	            initialize = true;
    	            return true;
	            } else {
	                DEBUG(("Docking Failed"));
	            }
	        }
	    }
	}
	if(isZone){
	    //default precision is 0.005
	    //fastMovement(actualDest,.63, 0.01);
	    api.setPositionTarget(actualDest);
	}else{
	    //api.setPositionTarget(actualDest);
	    api.setPositionTarget(actualDest);
	}
	api.setAttitudeTarget(dirVec);
    return false;
}


//Fastest possible movement given two parameters, current position and target position.
bool fastMovement(float targetPos[3],float distThresh, float precision) {
   float targetV[3];
   float ntargetV[3];
   mathVecSubtract(targetV, targetPos, currentPos, 3);
   /*
   //makes a negative velocity vector
   for(int j = 0; j < 3; j++) {
       ntargetV[j] = -targetV[j];
   }
   if(fminitialize) {
       fmdistance = mathVecMagnitude(targetV, 3);
       fminitialize = false;
   } else {
      if(mathVecMagnitude(targetV, 3) < distThresh * fmdistance) {
          if(mathVecMagnitude(currentVel, 3) > 0.05) {
              api.setVelocityTarget(ntargetV);
          } else  {
              api.setPositionTarget(targetPos);
          }
         
      } else {
           api.setVelocityTarget(targetV);
      }
   }
   */
   api.setPositionTarget(targetPos);
   if(mathVecMagnitude(targetV, 3) < precision) {
      fminitialize = true;
      return true;
    } else {
      return false;
    }
}

float distancecalc(float a[3], float b[3]) {
    float distance = sqrtf((mathSquare(a[0] - b[0]) + mathSquare(a[1] - b[1]) + mathSquare(a[2] - b[2])));
    return distance;
}

int optimumItem(){
    float testPos[3];
    float max = 0;
    int maxItem =7;
    for(int i=0;i<6;i++){
        if(i==blue){
            continue;
        }
        game.getItemLoc(testPos,i);
        float points;
        float dist = distancecalc(currentPos,testPos);
        if(i<2){
            points = .2;
        }else if(i<4){
            points = .15;
        }else{
            points = .1;
        }
        if(distancecalc(otherZone,testPos)<.055){
            points*=2;
        }
        float possible = (180-time-40*dist)*points;
        DEBUG(("%f",possible));
        if(possible>max){
            max = possible;
            maxItem = i;
        }
    }
    return maxItem;
}
//End page main
