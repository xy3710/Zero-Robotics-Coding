//Begin page main
float myState[12], otherState[12];
float myPos[3], *dest, SPSpos1[3], SPSpos2[3], SPSpos3[3];
float myVel[3], otherPos[3];
float item0[3], item1[3], item2[3], item3[3], item4[3], item5[3];
float myZone[4];
bool    newPath;              
bool    areWeThereYet;      
float   forces[3];      
float   checkPos[3];        
float   travelled[3];     
float   checkVec[3];    
float   targetVec[3];
float   from[3];
float   dest1[3];
bool initial;
bool dropSPS;
int itemId, item;
int time;

//SMDDDD
void init(){
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
	    
	    item0[0] = 0.23;
	    item0[1] = 0.23;
	    item0[2] = 0.23;
	    
	    item1[0] = -0.23;
	    item1[1] = -0.23;
	    item1[2] = -0.23;
	    
	    item2[0] = 0.36;
	    item2[1] = -0.36;
	    item2[2] = 0.36;
	    
	    item3[0] = -0.36;
	    item3[1] = 0.36;
	    item3[2] = -0.36;
	    
	    item4[0] = -0.50;
	    item4[1] = 0.50;
	    item4[2] = 0.50;	    
	    
	    item5[0] = 0.50;
	    item5[1] = -0.50;
	    item5[2] = -0.50;
	    api.getMyZRState(state);
        for(int i =0;i<3;i++){
            currentPos[i]=state[i];
            currentVel[i]=state[i+3];
            currentAtt[i]=state[i+6];
        }
        if(currentPos[1] < 0) {
            for(int i=0; i<3; i++) {
                SPSpos1[i] = -1 * SPSpos1[i];
                SPSpos2[i] = -1 * SPSpos2[i];
                SPSpos3[i] = -1 * SPSpos3[i];
            }
        }
        areWeThereYet = false;
        newPath = true;
        initial = true;
        itemId = 0;
        initialize = true;
        fminitialize = true;
        fmdistance = 0;
        docked =false;
        docked1 = false;
        docked2 = false;
        docked3 = false;
        dropped = false;
        dropped1 = false;
        dropped2 = false;
        dropped3 = false;
        dropSPS = true;
        game.dropSPS();
}

void loop() {
    api.getMyZRState(state);
    for(int i =0;i<3;i++){
        currentPos[i]=state[i];
        currentVel[i]=state[i+3];
        currentAtt[i]=state[i+6];
    }
    api.getMyZRState(myState);
    api.getOtherZRState(otherState);
    for (int i = 0; i < 3; i++)
        otherPos[i] = otherState[i];    
    if (initial)
        getItemDistance();
	//This function is called once per second.  Use it to control the satellite.
	if(game.getNumSPSHeld() == 2) {
        if(fastMovement(SPSpos2, 0.6, 0.15)) {
            //DEBUG(("Dropped SPS2"));
            game.dropSPS();   
        } else if(time > 30) {
            game.dropSPS();
        }
    }
    if(game.getNumSPSHeld() == 1) {
        if(fastMovement(SPSpos3, 0.6, 0.15)) {
            //DEBUG(("Dropped SPS3"));
            game.dropSPS();
            itemId = getClosestItem();
            if(itemId == 6) {
                dropped = true;
                docked = true;
            }
        } else if(time > 60) {
            game.dropSPS();
            itemId = getClosestItem();
            if(itemId == 6) {
                dropped = true;
                docked = true;
            }
        }
    }
    if(game.getNumSPSHeld() == 0) {
        //DEBUG(("Finished dropping SPS."));
        if(docked){
            game.getZone(myZone);
            if(dropped){
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
            }else{
                if(dockItem(itemId,true)){
                    dropped = true;
                    itemId = 1-itemId;
                }
            }
        }else{
            if(dockItem(itemId,false)){
                docked = true;
            }
        }
    }
    time++;
}

bool fminitialize,initialize; //tells method if initializen is required for first loop through
float firstPos[3],maxDist,minDist,dist,drop,currentVel[3],currentAtt[3],currentPos[3],loadDest[3],state[12],actualDest[3],dirVec[3],pointVec[3]; //required variables for run
float itemPos[3]; //item position
bool reached,pointing; //boolean is true if item dock position is reached
bool docked,docked1,dropped,dropped1,docked2,dropped2,docked3,dropped3;
float fmdistance;

bool dockItem(int id,bool isZone){
    if(isZone){
        itemPos[0] = myZone[0];
        itemPos[1] = myZone[1];
        itemPos[2] = myZone[2];
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
            maxDist = .205;
            minDist = .125;
        }
        initialize = false;
    }
    reached=false;
    pointing = false;
    for(int i =0;i<3;i++){
        loadDest[i] = itemPos[i];
        pointVec[i]=currentAtt[i];
    }
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
   //makes a negative velocity vector
   for(int j = 0; j < 3; j++) {
       ntargetV[j] = -targetV[j];
   }
   /*if(fminitialize) {
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
   }*/
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

void hasReachedTargetSlow(float dest[3], float closeEdge)
{
        float distance;
 
    //    DEBUG(("myPos is %f, %f, %f", myState[0], myState[1], myState[2]));
      //  DEBUG(("myVel is %f, %f, %f", myState[3], myState[4], myState[5]));
        if(newPath) {
            memcpy(from, myPos, 3 * sizeof(float));
            newPath = false;
        }
        mathVecSubtract(targetVec, dest, from, 3);
        mathVecSubtract(travelled, myPos, from, 3);
        distance = mathVecMagnitude(targetVec, 3) - mathVecMagnitude(travelled, 3);
        //DEBUG(("travelled is %f", 2*mathVecMagnitude(travelled, 3)));
        //DEBUG(("target is %f", mathVecMagnitude(targetVec, 3)));
        //DEBUG(("distance is %f", distance));
        if (distance < 0.1f) {
            api.setAttitudeTarget(dest);
            if(fabsf(distance) < closeEdge) {
                areWeThereYet = true;
            }
        } else
            api.setAttitudeTarget(dest);
        api.setPositionTarget(dest);
        return;
}

void getItemDistance()
{
    float itemDis[3];
    mathVecSubtract(itemDis, myPos, item0, 3);
    mathVecSubtract(itemDis, myPos, item1, 3);
    mathVecSubtract(itemDis, myPos, item2, 3);
    mathVecSubtract(itemDis, myPos, item3, 3);
    mathVecSubtract(itemDis, myPos, item4, 3);
    mathVecSubtract(itemDis, myPos, item5, 3);
    mathVecSubtract(itemDis, myPos, otherPos, 3);
}
int getClosestItem() {
    api.getMyZRState(state);
    for(int i =0;i<3;i++){
        myPos[i]=state[i];
    }
    float abc[3];
    float distance = 100.0;
    float dist;
    int itemid = 6;
    float itemDis[3];
    for(int i=0; i<6; i++) {
        if(game.hasItemBeenPickedUp(i) == false) {
            game.getItemLoc(abc, i);
            dist = distancecalc(abc, myPos);
            //DEBUG(("Distance to item %d: %f",i,dist));
            if(dist < distance) {
                distance = dist;
                itemid = i;
            }
        }
    }
    //DEBUG(("Closest item: %d", itemid));
    return itemid;
}
void hasReachedTargetFast(float dest[3], float closeEdge) {
    if(areWeThereYet) {
     //   api.setAttitudeTarget(dest);
        api.setPositionTarget(dest);
        //DEBUG(("myPos is %f, %f, %f", myState[0], myState[1], myState[2]));
        //DEBUG(("myVel is %f, %f, %f", myState[3], myState[4], myState[5]));
        //DEBUG(("areWeThereYet is set"));
        if(fabsf(mathVecMagnitude(travelled, 3) - mathVecMagnitude(targetVec, 3)) < closeEdge) {
            areWeThereYet = false;
            newPath = true;
        }
        return;
    }
    
    if(newPath) {
        memcpy(from, myPos, 3 * sizeof(float));
        mathVecSubtract(targetVec, dest, from, 3);
        for(int i = 0; i < 3; i++)
            forces[i] = targetVec[i];
        newPath = false;
    }
    mathVecSubtract(travelled, myPos, from, 3);
 //   mathVecSubtract(targetVec, dest, from, 3);
    //DEBUG(("myPos is %f, %f, %f", myState[0], myState[1], myState[2]));
    //DEBUG(("travelled is %f", 2*mathVecMagnitude(travelled, 3)));
    //DEBUG(("target is %f", mathVecMagnitude(targetVec, 3)));
    if((2*mathVecMagnitude(travelled, 3)) >= (mathVecMagnitude(targetVec, 3))) {
        if(fabsf(mathVecMagnitude(travelled, 3) - mathVecMagnitude(targetVec, 3)) < 0.1f) {
         //   api.setAttitudeTarget(dest);
            api.setPositionTarget(dest);
            newPath = true;
            areWeThereYet = true;
            //DEBUG(("reached"));
            //DEBUG(("myPos is %f, %f, %f", myState[0], myState[1], myState[2]));
            //DEBUG(("myVel is %f, %f, %f", myState[3], myState[4], myState[5]));
            if(fabsf(mathVecMagnitude(travelled, 3) - mathVecMagnitude(targetVec, 3)) < closeEdge) {
                areWeThereYet = false;
                newPath = true;
            }
            return;
        } else {
            for(int i = 0; i < 3; i++)
                forces[i] = 0 - targetVec[i];
            //DEBUG(("set forces negative"));
        }
    }
        
    mathVecNormalize(forces, 3);
    //DEBUG(("myPos is %f, %f, %f", myState[0], myState[1], myState[2]));
    //DEBUG(("myVel is %f, %f, %f", myState[3], myState[4], myState[5]));
// mathVecSubtract(travelled, myPos, from, 3);
//    if(fabsf(mathVecMagnitude(travelled, 3) - mathVecMagnitude(targetVec, 3)) > 0.1f) 
        api.setForces(forces);
//    else
 //        api.setPositionTarget(dest); 
    //DEBUG(("force is %f, %f, %f", forces[0], forces[1], forces[2]));
    return;
}
//End page main
//End page main
