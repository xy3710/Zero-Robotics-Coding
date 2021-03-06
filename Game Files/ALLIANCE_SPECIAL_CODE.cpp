//Begin page calcSPS
// Page,
//     _____ ____  _____    ______      __           __      __  _                 
//    / ___// __ \/ ___/   / ____/___ _/ /______  __/ /___ _/ /_(_)___  ____  _____
//    \__ \/ /_/ /\__ \   / /   / __ `/ / ___/ / / / / __ `/ __/ / __ \/ __ \/ ___/
//   ___/ / ____/___/ /  / /___/ /_/ / / /__/ /_/ / / /_/ / /_/ / /_/ / / / (__  ) 
//  /____/_/    /____/   \____/\__,_/_/\___/\__,_/_/\__,_/\__/_/\____/_/ /_/____/  
//Calculating the Location of the second SPS based on SPS area and item attitude 

//SPS constants
#define SPS_AREA    .115f
#define OFFSET      .17f

#define DIST(POINT1, POINT2) sqrtf( ((POINT2[0] - POINT1[0]) * (POINT2[0] - POINT1[0])) +   \
                                    ((POINT2[1] - POINT1[1]) * (POINT2[1] - POINT1[1])) +   \
                                    ((POINT2[2] - POINT1[2]) * (POINT2[2] - POINT1[2])))

void calcSPS(){
    //Calculate SPS circle
	for(int i = 0; i < 3; i++){
	    offset[i] = itemsLoc[id][i] + OFFSET*itemsAtt[id][i];
	    circ[i] = (pos[i] + offset[i])/2;
	    base[i] = offset[i] - pos[i];
	}
	
	rad = 2*SPS_AREA/mathVecMagnitude(base, 3);
	
	//Get point vectors on SPS circle
    circs[0][0] = base[0]; circs[0][1] = base[1];
    circs[0][2] = -((base[0]*base[0])+(base[1]*base[1]))/base[2];
    mathVecNormalize(circs[0],3);
    circs[2][0] = base[1]/base[0]; circs[2][1] = -1; circs[2][2] = 0;
    mathVecNormalize(circs[2],3);
    
    for(int i = 0; i < 3; i++){
        circs[0][i]*=rad;
        circs[2][i]*=rad;
        circs[1][i] = -1*circs[0][i];
        circs[3][i] = -1*circs[2][i];
        offset[i] = itemsLoc[id][i] + 2*OFFSET*itemsAtt[id][i];
    }
    
    //Combine point vectors with circle center
    mathVecAdd(circs[0], circ, circs[0],3);
    mathVecAdd(circs[1], circ, circs[1],3);
    mathVecAdd(circs[2], circ, circs[2],3);
    mathVecAdd(circs[3], circ, circs[3],3);
    
    //Get optimal circle point
    float min = 420;
    
    for(int i = 0; i < 4; i++){
        if(DIST(circs[i],offset) < min){
            min = (DIST(circs[i], offset));
            memcpy(placement, circs[i], sizeof(placement) + 1);
        }
    }
    
    memcpy(lastSPS, itemsLoc[id], sizeof(lastSPS) + 1);
}
//End page calcSPS
//Begin page docking
// Page,
//      ____             __   _            
//     / __ \____  _____/ /__(_)___  ____ _
//    / / / / __ \/ ___/ //_/ / __ \/ __ `/
//   / /_/ / /_/ / /__/ ,< / / / / / /_/ / 
//  /_____/\____/\___/_/|_/_/_/ /_/\__, /  
//                                /____/   
//Docking items/adapters and dropping items in zones

//Threshold constants
#define    L_MAXDIST  .173f
#define    L_MINDIST  .151f

#define    M_MAXDIST  .160f
#define    M_MINDIST  .138f

#define    S_MAXDIST  .146f
#define    S_MINDIST  .124f

#define    Z_MAXDIST  .19f
#define    Z_MINDIST  .10f

#define adapterThresh   .1f

bool dock(int id, bool other){
    //Item/Zone location
    float loc[3];
    
    //Threshold to dock
    float minmax[2];
    
    //Offset vector
    float tol[3];
    
    //Dock location
    float dest[3];
    
    //Target attitude
    float atttarget[3];
    
    //Temporary array for calculations
    float temp[3];
    
    //Distance from item
    float dist;
    
    //Criteria bools
    bool isZone = false;
    bool isSpecial = false;
    
    bool reached = false;
    bool pointing = false;
    bool side = false;
    
    if(other){
        //Zone/Item thresholds
        if(game.getItemType(id) == ITEM_TYPE_SPECIAL){
            minmax[0] = S_MINDIST;
            minmax[1] = S_MAXDIST;
            
            //Item is special
            isSpecial = true;
            
            //Set location
            memcpy(loc, itemsLoc[id], sizeof(loc));
        }else{
            minmax[0] = Z_MINDIST;
            minmax[1] = Z_MAXDIST;
            
            //Target is zone
            isZone = true; 
            
            //Set location to zone
            for(int i = 0; i < 3; i++){
                loc[i] = zone[i];
            }
        }
        //Set offset vector
        mathVecSubtract(temp, pos, loc, 3);
        mathVecNormalize(temp, 3);
        for(int i = 0; i < 3; i++){
            tol[i] = temp[i]*(minmax[1] - .05);
        }
        side = true;
    }else{
        //Set location
        memcpy(loc, itemsLoc[id], sizeof(loc));
        
        //Set thresholds
        if(game.getItemType(id) == ITEM_TYPE_LARGE){
            minmax[0] = L_MINDIST;
            minmax[1] = L_MAXDIST;
        }
        
        if(game.getItemType(id) == ITEM_TYPE_MEDIUM){
            minmax[0] = M_MINDIST;
            minmax[1] = M_MAXDIST;
        }
        
        if(game.getItemType(id) == ITEM_TYPE_SMALL){
            minmax[0] = S_MINDIST;
            minmax[1] = S_MAXDIST;
        }
        
        //Set tolerance vector
        for(int i = 0; i < 3; i++){
            tol[i] = itemsAtt[id][i]*(minmax[1]-.01);
        }   
    }
    
    //Set destination coords
    mathVecAdd(dest, loc, tol, 3);
    
    //Set target attitude
    mathVecNormalize(tol, 3);
    for(int i = 0; i < 3; i++){
        atttarget[i] = -1*tol[i];
    }
    
    //Calculate distance
    mathVecSubtract(temp, pos, loc, 3);
    dist = mathVecMagnitude(temp, 3);
    
    //Check if within range
    if(dist > minmax[0]+ .005 && dist < minmax[1] - .005){
        reached = true;
    }

    //Check if pointing at item
    if(acosf(mathVecInner(att, atttarget, 3)) < .2){
        pointing = true;
    }
    
    //Check if on right side
    if(game.isFacingCorrectItemSide(id)){
        side = true;
    }
    
    //Dock/Drop item if conditions are met
    if(reached && pointing && side){
        if(isZone){
            game.dropItem();
            return true;
        }else{
            if(game.dockItem())
                return true;
        }
    }
    //Go to target position and attitude
    api.setPositionTarget(dest);
    api.setAttitudeTarget(atttarget);
    
    return false;
}

bool dockAdapter(int adapter){
    //Target through adapter
    float extra[3];
    
    //Get vector between
    for(int i = 0; i < 3; i++)
        extra[i] = (itemsLoc[adapter][i] - pos[i]);
        
    //Calculate vector through
    mathVecNormalize(extra, 3);
    for(int i = 0; i < 3; i++)
        extra[i] *= adapterThresh;
        
    //Set target coordinates through
    mathVecAdd(extra, extra, itemsLoc[adapter], 3);
    
    //Set position to behind adapter
    api.setPositionTarget(extra);
    
    //Check if adapter has been reached
    if(game.hasAdapter()){
        return true;
    }
    return false;
}

//End page docking
//Begin page main
// Go team,
//     ______           __       _       __                                  __     
//    / ____/________ _/ /_     | |     / /___ _____  ____  ____ ___  ____ _/ /_  __
//   / /   / ___/ __ `/ __ \    | | /| / / __ `/ __ \/ __ \/ __ `__ \/ __ `/ / / / /
//  / /___/ /  / /_/ / /_/ /    | |/ |/ / /_/ / / / / /_/ / / / / / / /_/ / / /_/ / 
//  \____/_/   \__,_/_.___/     |__/|__/\__,_/_/ /_/\____/_/ /_/ /_/\__,_/_/\__, /  
//                                                                         /____/ 

//Item locations
float itemsLoc[9][3];

//Item attitude
float itemsAtt[9][3];

//ZR state
float state[12];
float pos[3];
float att[3];

//Red or Blue
bool isRed;

//SPS circle
float circ[3];
float rad;
float offset[3];
float base[3], circs[4][3];

//Optimal sps
float placement[3];
float lastSPS[3];

//Target item
int id;
int item;
int adapter;

//Zone location
float zone[3];

//Game status
// 0: 1st SPS
// 1: 2nd SPS
// 2: 3rd SPS
int specialStatus;
int status;

bool gotSpecial;
bool getSpecial;

float gameBounds[3];

#define DIST(POINT1, POINT2) sqrtf( ((POINT2[0] - POINT1[0]) * (POINT2[0] - POINT1[0])) +   \
                                    ((POINT2[1] - POINT1[1]) * (POINT2[1] - POINT1[1])) +   \
                                    ((POINT2[2] - POINT1[2]) * (POINT2[2] - POINT1[2])))

#define INRANGE(POINT1, POINT2) (DIST(POINT1, POINT2) < .05)


void init(){
    id = 0;
    
    //Set initial statuses to 1
    status = 0;
    specialStatus = 0;
    
    //Special item booleans
    gotSpecial = false;
    getSpecial = true;
    
    //Get item states
    updatePos();
	
	if(pos[1] > 0){
	    isRed = false;
	    id = 0;
	}else{
	    isRed = true;
	    id = 1;
	}
	
	//Calculate SPS pos
	calcSPS();
}

void loop(){
    
    DEBUG(("Status: %d", status));
	    
	//Update pos
	updatePos();
	
    if(specialStatus > 0){
        //Docking adapter
        if(specialStatus == 1){
            if(dockAdapter(adapter)){
                specialStatus = 2;
            }
        }
        //Special item
        if(specialStatus == 2){
            if(dock(6, true)){
                game.dropItem();
                specialStatus = 0;
                gotSpecial = true;
            }
        }
    }else{
        //First SPS
        if(status == 0){
            if(isRed)
                item = 1;
            else
                item = 0;
            adapter = item + 7;
            game.dropSPS();
            status = 1;
        }
        //Second SPS
        if(status == 1){
            game.dropItem();
            api.setPositionTarget(placement);
            if(INRANGE(placement, pos)){
                game.dropSPS();
                status = 2;
            }
        }
        //Dock first item
        if(status == 2){
            if(dock(item, false)){
                status = 3;
            }
        }
        //Drop third SPS
        if(status == 3){
            game.dropSPS();
            game.getZone(zone);
            status = 4;
        }
        //Drop first item
        if(status == 4){
            if(dock(0, true)){
                status = 5;
                item = 1 - item;
            }else{
                specialStatus = 0;
            }
        }
        //Dock second item
        if(status == 5){
            if(dock(item, false)){
                status = 6;
            }
        }
        //Drop second item
        if(status == 6){
            if(dock(0, true)){
                status = 7;
                specialStatus = getSpecial;
            }
        }
    }
}

//Update item and ZR positions
void updatePos(){
    api.getMyZRState(state);
    
	for(int i = 0; i < 3; i++){
        pos[i] = state[i];
        att[i] = state[i+6];
	}
	
    for(int i = 0; i < 9 ; i++){
	    game.getItemZRState(state, i);
	    
	    for(int j = 0; j < 3; j++){
	        itemsLoc[i][j] = state[j];
	        itemsAtt[i][j] = state[j+6];
	    }
	}
}


//End page main
