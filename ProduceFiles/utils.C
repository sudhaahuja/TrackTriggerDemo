bool requirePtCut3 = true;
bool requireBlinding = true;
bool SkipEventWithEmptyLayer = false;  //set it to false for PU only samples
bool multiBx = false;
bool halfLayer = false; //only simulate data for half layer 5

unsigned int decodeLayer( unsigned int moduleId) {
   return moduleId / 10000;
}

unsigned int decodeLadder( unsigned int moduleId) {
   return (moduleId / 100) % 100; 
}

unsigned int decodeModule( unsigned int moduleId) {
   return moduleId % 100; 
}

bool pass3GeVCut(unsigned moduleId, float bend) {
   bool passCut=false;
   unsigned int lay = decodeLayer(moduleId); //layer
   unsigned int lad = decodeLadder(moduleId); //phi module for barrel; r rings for endcaps
   switch (lay) {
      case 5: if (fabs(bend)<=1.5) {/*std:cout<<"bend="<<bend<<", "<<std::endl; */passCut=true; } break;
      case 6: if (fabs(bend)<=1.5) passCut=true;break;
      case 7: if (fabs(bend)<=2.0) passCut=true;break;
      case 8: if (fabs(bend)<=3.0) passCut=true;break;
      case 9: if (fabs(bend)<=3.5) passCut=true;break;
      case 10: if (fabs(bend)<=4.5) passCut=true;break;
      case 11:  
      case 18:  
                  if (lad<=1 && (fabs(bend)<=1)) passCut=true;
                  if (lad>1 && lad<=4 && (fabs(bend)<=1.5)) passCut=true;
                  if (lad>4 && lad<=7 && (fabs(bend)<=2)) passCut=true;
                  if (lad>7 && lad<=8 && (fabs(bend)<=2.5)) passCut=true;
                  if (lad>8 && lad<=9 && (fabs(bend)<=3)) passCut=true;
                  if (lad>9 && lad<=10 && (fabs(bend)<=2)) passCut=true;
                  if (lad>10 && lad<=11 && (fabs(bend)<=2.5)) passCut=true;
                  if (lad>11 && lad<=13 && (fabs(bend)<=3)) passCut=true;
                  if (lad>13 && lad<=14 && (fabs(bend)<=3.5)) passCut=true;
                  break;
      case 12:  
      case 19: 
                  if (lad<=1 && (fabs(bend)<=1)) passCut=true;
                  if (lad>1 && lad<=5 && (fabs(bend)<=1.5)) passCut=true;
                  if (lad>5 && lad<=8 && (fabs(bend)<=2)) passCut=true;
                  if (lad>8 && lad<=9 && (fabs(bend)<=3)) passCut=true;
                  if (lad>9 && lad<=11 && (fabs(bend)<=2)) passCut=true;
                  if (lad>11 && lad<=12 && (fabs(bend)<=2.5)) passCut=true;
                  if (lad>12 && lad<=14 && (fabs(bend)<=3)) passCut=true;
                  break;   
      case 13:  
      case 20: 
                  if (lad<=2 && (fabs(bend)<=1)) passCut=true;
                  if (lad>2 && lad<=6 && (fabs(bend)<=1.5)) passCut=true;
                  if (lad>6 && lad<=8 && (fabs(bend)<=2)) passCut=true;
                  if (lad>8 && lad<=9 && (fabs(bend)<=2.5)) passCut=true;
                  if (lad>9 && lad<=10 && (fabs(bend)<=3)) passCut=true;
                  if (lad>10 && lad<=12 && (fabs(bend)<=2)) passCut=true;
                  if (lad>12 && lad<=13 && (fabs(bend)<=2.5)) passCut=true;
                  if (lad>13 && lad<=14 && (fabs(bend)<=3)) passCut=true;
                  break;
      case 14:  
      case 21: 
                  if (lad<=4 && (fabs(bend)<=1)) passCut=true;
                  if (lad>4 && lad<=7 && (fabs(bend)<=1.5)) passCut=true;
                  if (lad>7 && lad<=9 && (fabs(bend)<=2)) passCut=true;
                  if (lad>9 && lad<=10 && (fabs(bend)<=2.5)) passCut=true;
                  if (lad>10 && lad<=14 && (fabs(bend)<=2)) passCut=true;
                  break;
      case 15: 
      case 22: 
                  if (lad<=6 && (fabs(bend)<=1)) passCut=true;
                  if (lad>6 && lad<=8 && (fabs(bend)<=1.5)) passCut=true;
                  if (lad>8 && lad<=10 && (fabs(bend)<=2)) passCut=true;
                  if (lad>10 && lad<=11 && (fabs(bend)<=2.5)) passCut=true;
                  if (lad>11 && lad<=14 && (fabs(bend)<=2)) passCut=true;
                  break;
      default: break;
   }   
   return passCut;
}


