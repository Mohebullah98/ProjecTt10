#include <iostream>
#include <fstream>
#include <cmath>
using namespace std;
class Image{
  public:
  int numRows;
  int numCols;
  int minVal;
  int maxVal;
  int **imgAry;
  int **ccAry;
  void zeroFramed(int ** imgAry){
    for(int i=0;i<numRows+2;i++){
      for(int j=0;j<numCols+2;j++){
        imgAry[i][j] =0;
      }
    }
  }
  void loadImage(ifstream & inFile, int ** zeroFramedAry){
    for(int i=1;i<numRows+1;i++){
      for(int j=1;j<numCols+1;j++){
        inFile>>zeroFramedAry[i][j];
        
      }
    }
  }

};
class connectCC: virtual public Image{
  public:
  int label;
  int numbPixels;
  struct boundingBox{
    int minRow;
    int minCol;
    int maxRow;
    int maxCol;
    int label;
  };
  struct boundingBox *Bound;
  Image I;
  void clearCC(int **ccAry){
    for(int i=0;i<I.numRows+2;i++){
      for(int j=0;j<I.numCols+2;j++){
        ccAry[i][j]=0;
      }
    }
  }
  void loadCC(int cclabel){
    for(int i=1;i<I.numRows+1;i++){
      for(int j=1;j<I.numCols+1;j++){
        if(I.imgAry[i][j]==cclabel){
          I.ccAry[i][j]=cclabel;
          
        }
      }
    }
  }
};
  class chainCode:public connectCC{
    public:
    class point{
      public:
      int row;
      int col;
    };
    int* NeighborCoord;
    point startP;
    point currentP;
    point nextP;
    int lastQ;
    Image I;
    connectCC C;
    int * zeroTable;
    int nextDir;
    int pchainDir;

    void getChainCode(connectCC::boundingBox b, int** ccAry,ofstream & debugFile,ofstream& chainCodeFile){
      int label = b.label;
      for(int i=b.minRow;i<b.maxRow+1;i++){
        for(int j=b.minCol;j<b.maxCol+1;j++){
          if (ccAry[i][j]==label){
            debugFile<<i<<" "<<j<<" "<<ccAry[i][j]<<endl;
            chainCodeFile<<i<<" "<<j<<" "<<ccAry[i][j]<<" ";
            startP.row=i;
            startP.col=j;
            currentP.row=i;
            currentP.col=j;
            lastQ=4;
            i=b.maxRow+1;
            j=b.maxCol+1;
          }
        }
      }int c =0;
          while(nextP.row!=startP.row||nextP.col!=startP.col){
          int pchainDir = (lastQ+1) % 8;//direction
          int dummy=0;
          pchainDir=findNextP(pchainDir,currentP.row,currentP.col);
          chainCodeFile<<pchainDir;
          if(c==20){
             debugFile<<endl;
             c=0;
          }
          debugFile<<pchainDir<<" ";
          currentP.row=nextP.row;
          currentP.col=nextP.col;
          lastQ=zeroTable[(pchainDir+7)%8];
          c++;
          }
        
      
    }
    
    int findNextP(int &pchainDir,int i, int j){
      int count=0;
      while(count<8){
          pchainDir =(++lastQ)%8;
        switch(pchainDir){
          case 0:
          if(I.ccAry[i][j+1]>0){ nextP.row =i; nextP.col =j+1; return 0; break;}
          case 1:
          if(I.ccAry[i-1][j+1]>0){nextP.row=i-1; nextP.col=j+1;return 1;break;}
          case 2:
          if(I.ccAry[i-1][j]>0){nextP.row=i-1;nextP.col=j; return 2;break;}
          case 3:
          if(I.ccAry[i-1][j-1]>0){nextP.row=i-1;nextP.col=j-1;return 3;break;}
          case 4:
          if(I.ccAry[i][j-1]>0){nextP.row=i;nextP.col=j-1; return 4;break;}
          case 5:
          if(I.ccAry[i+1][j-1]>0){nextP.row=i+1;nextP.col=j-1; return 5;break;}
          case 6:
          if(I.ccAry[i+1][j]>0){nextP.row=i+1;nextP.col=j; return 6;break;}
          case 7:
          if(I.ccAry[i+1][j+1]>0){nextP.row=i+1;nextP.col=j+1; return 7; break;}
          default:break;
        }
        count++;
      }
    }
   
    void prettyPrint(int **Ary,ofstream & outFile){
      for(int i=0;i<I.numRows+2;i++){
        for(int j=0;j<I.numCols+2;j++){
          if(Ary[i][j]>0) outFile<<Ary[i][j];
        }
      }
      outFile<<endl;
    } 
  };


int main(int argc, char* argv[]){
  Image I;
  connectCC C;
  chainCode ch;
  
  ifstream LabelFile;
  ifstream propertyFile;
  LabelFile.open(argv[1]);
  propertyFile.open(argv[2]);
  LabelFile>>I.numRows>>I.numCols>>I.minVal>>I.maxVal;
  ofstream debugFile;
  ofstream chainCodeFile;
  chainCodeFile.open(argv[3]);
  debugFile.open(argv[4]);
  chainCodeFile<<I.numRows<<" "<<I.numCols<<" "<<I.minVal<<" "<<I.maxVal<<endl;
  debugFile<<I.numRows<<" "<<I.numCols<<" "<<I.minVal<<" "<<I.maxVal<<endl;
  ch.NeighborCoord = new int[8]();
  I.imgAry= new int*[I.numRows+2]();
  for(int i=0;i<I.numRows+2;i++){
    I.imgAry[i]= new int[I.numCols+2]();
  }
  I.ccAry= new int*[I.numRows+2]();
  for(int i=0;i<I.numRows+2;i++){
    I.ccAry[i]= new int[I.numCols+2]();
  }
  ch.zeroTable = new int[8]{6,0,0,2,2,4,4,6};
int rows,cols,min,max;
int numCC;
int label;
int count;
I.loadImage(LabelFile,I.imgAry);
propertyFile>>rows>>cols>>min>>max>>numCC;
C.Bound = new connectCC::boundingBox[numCC];
C.I = I;
  ch.I=I;
  ch.C=C;
for(int i=0;i<numCC;i++){
  propertyFile>>C.Bound[i].label;
  propertyFile>>count;
  propertyFile>>C.Bound[i].minRow;
  propertyFile>>C.Bound[i].minCol;
  propertyFile>>C.Bound[i].maxRow;
  propertyFile>>C.Bound[i].maxCol;

C.clearCC(I.ccAry);
C.loadCC(C.Bound[i].label);

ch.getChainCode(C.Bound[i], I.ccAry,debugFile,chainCodeFile);
if(i!=numCC-1)debugFile<<endl<<":"<<endl<<":"<<endl<<"###"<<endl;
chainCodeFile<<endl;
}
LabelFile.close();
propertyFile.close();
chainCodeFile.close();
debugFile.close();
}