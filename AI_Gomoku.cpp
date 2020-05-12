#include<algorithm> //max,min
#include<iostream>
#include<functional>
#include<stdlib.h>
#include<string>
#include<unistd.h>
#include<chrono> //time check
#include<time.h> //clock_t, CLOCKS_PER_SEC

using namespace std;

#define max_depth 100000000
#define INF 100000000
#define NINF -10000000
			
///////////////global variables/////////////////////////////
////////////////////////////////	
char player_color, ai_color; //player와 AI의 돌 색깔 저장
char cur_state[400]; 
///////////////function declarations///////////////////////////////
void show_state(char[400]); //현재 state 상태를 보여주는 함수
int action_isvalid(char [400],int,int);
int check_33(char[400],char,int,int,int,int,int,int,int,int);

int start(char[400]);
int player_turn(char[400]);
int terminal_test(char[400]); //main에서 사용
int estimate(int,char[400]);


//main에서 termiinal test 위해 필요
int horizontal_win(char[400],char);
int vertical_win(char[400],char);
int Rdiagonal_win(char[400],char);
int Ldiagonal_win(char[400],char);

//evaluation function에 필요
int five_in_row(char[400],int); //player or ai의 우승 생성
int block_player_5(char[400],int); //player의 우승 차단
int make_open_4(char[400],int); //ai의 열린 4 생성
int block_player_4(char[400],int); //player의 열린 4 차단
int block_player_3(char[400],int); //player의 열린 3 차단
int block_closed_3(char[400],int); //player의 막힌 4 차단
int make_open_3(char[400],int); //ai의 열린 3 생성
int make_open_2(char[400],int); //ai의 열린 2 생성

int Iterative_Deepening_Search(char[400]); //action number를 return
int Find_action(char[400],int,int*);
int Alpha_Beta_Search(int,char[400],int,int, int, int*,int);//returns action
///////////////////////////////////////////////////////////
void show_state(char state[400]){
	
	cout<<' '<<' ';
	for(int i=1;i<20;i++){
		cout<<i<<' ';
		if(i<10)
			cout<<' ';
	}
	cout<<'\n';
	for(int i=1;i<20;i++){
		cout<<char(65+i-1)<<' ';
		for(int j=1;j<20;j++){
			cout<<state[(i-1)*19+(j-1)]<<' '<<' ';			
		}
		cout<<'\n';
	}
}
//////////////////////////////////////////////////////////
int action_isvalid(char state[400],int action,int player){ //놓을 수 있는 자리인지, 3-3이 되는 action인지 검사
	char color;
	if(player==1) color=player_color;
	else color=ai_color;

	if(state[action]!='-') //빈 자리가 아님
		return 0;
		
	
	//2칸 이내에 돌이 없으면 invalid(제일 가까운게 3칸 떨어진거)
	int check=0;
	int x=action/19; int y=action%19;
	for(int i=x-2;i<=x+2;i++){
		for(int j=y-2;j<=y+2;j++){
			if(i>=0&&i<19&&j>=0&&j<19){
				if(state[i*19+j]!='-'){
					check=1;
					break;
				}
			}
		}
	}	
	if(!check && player==0){ //이 조건은 ai만 검사
		return 0;
	}
	
	//3-3 되는지 검사
	int p1,p2,p3,p4,p5,p6,p7,p8;
	int line_count=0;
	//horizontal line check
	p1=action-1; p2=action-2; p3=action-3; p4=action+1; p5=action+2; p6=action+3; p7=p1-1; p8=p6+1;
	if(check_33(state,color,p1,p2,p3,p4,p5,p6,p7,p8)){
		line_count+=1;
	}
	//vertical line check
	p1=action-19; p2=action-19*2; p3=action-19*3; p4=action+19; p5=action+19*2; p6=action+19*3; p7=p1-19; p8=p6+19;
	if(check_33(state,color,p1,p2,p3,p4,p5,p6,p7,p8)) {
		line_count+=1;
		if(line_count>=2) return 0;
	}
	//Rdiagonal line check
	p1=action-18; p2=action-18*2; p3=action-18*3; p4=action+18; p5=action+18*2; p6=action+18*3; p7=p1-18; p8=p6+18;
	if(check_33(state,color,p1,p2,p3,p4,p5,p6,p7,p8)) {
		line_count+=1;
		if(line_count>=2) return 0;
	}
	//Ldiagonal line check
	p1=action-20; p2=action-20*2; p3=action-20*3; p4=action+20; p5=action+20*2; p6=action+20*3; p7=p1-20; p8=p6+20;
	if(check_33(state,color,p1,p2,p3,p4,p5,p6,p7,p8)) {
		line_count+=1;
		if(line_count>=2) return 0;
	}	
	
	//제약조건 다 통과->valid
	return 1;
}
//////////////////////////////////////////////////////////
int check_33(char state[400],char color,int p1,int p2,int p3,int p4,int p5,int p6,int p7,int p8){ 
	//33 position이면 return 1, 아니면 return 0
	if(state[p1]==color){
		if(state[p2]==color) 
			if(state[p3]=='-' && state[p4]=='-') return 1; //1,2
		else if(state[p2]=='-'){
			if(state[p3]==color)
				if(state[p7]=='-' && state[p4]=='-') return 1; //1,3
		}
		if(state[p4]==color) 
			if(state[p2]=='-' && state[p5]=='-') return 1; //1,4
		else if(state[p4]=='-'){
			if(state[p5]==color) 
				if(state[p2]=='-' && state[p6]=='-') return 1; //1,5
		}
	}
	else if(state[p1]=='-'){
		if(state[p2]==color){
			if(state[p3]==color) 
				if(state[p7]=='-' && state[p4]=='-') return 1; //2,3
			if(state[p4]==color)
				if(state[p3]=='-' && state[p5]=='-') return 1; //2,4
		}
		if(state[p4]==color){
			if(state[p5]==color && state[p6]=='-') return 1; //4,5
			else if(state[p5]=='-'){
				if(state[p6]==color && state[p8]=='-') return 1; //4,6
			}
		}
		else if(state[p4]=='-'){
			if(state[p5]==color && state[p6]==color && state[p8]=='-') return 1; //5,6
		}
	}
	//else
	return 0;	
}
//////////////////////////////////////////////////////////
int horizontal_win(char state[400],char color){
	int count=0;
	int result=0;
	
	for(int i=0;i<19;i++){
		for(int j=0;j<19;j++){
			if(state[i*19+j]==color){
				count++;
				if(count==2) result=2;
				if(count==3) result=3; //각각 가중치 부여(3개연속)
				if(count==4) result=4; //4개연속
				if(count==5) result=5; //5개연속 -> win,terminate 조건
			}
			else
				count=0;
		}
		count=0;
	}

	return result;
}
///////////////////////////////////////////////////////////
int vertical_win(char state[400],char color){
	int count=0;
	int result=0;
	
	for(int j=0;j<19;j++){
		for(int i=0;i<19;i++){
			if(state[i*19+j]==color){
				count++;
				if(count==2) result=2;
				if(count==3) result=3;
				if(count==4) result=4;
				if(count==5) result=5;
			}
			else
				count=0;
		}
		count=0;
	}

	return result;
}
///////////////////////////////////////////////////////////
int Rdiagonal_win(char state[400],char color){
	int count=0;
	int result=0;	
	
	for(int i=0;i<19;i++){
		int j=0;
		int x=i; int y=j;
		while(y<19&&x<19){
			if(state[x*19+y]==color){
				count++;
				if(count==2) result=2;
				if(count==3) result=3;
				if(count==4) result=4;
				if(count==5) result=5;
			}
			else
				count=0;
			
			x+=1; y+=1;
		}
		count=0;
	}
	
	for(int j=1;j<19;j++){
		int i=0;
		int x=i; int y=j;
		while(x<19&&y<19){
			if(state[x*19+y]==color){
				count++;
				if(count==2) result=2;
				if(count==3) result=3;
				if(count==4) result=4;
				if(count==5) result=5;
			}
			else
				count=0;
			
			x+=1; y+=1;
		}
		count=0;
	}	

	return result;
}
///////////////////////////////////////////////////////////
int Ldiagonal_win(char state[400],char color){
	int count=0;
	int result=0;
	
	for(int i=0;i<19;i++){
		int j=19;
		int x=i; int y=j;
		while(x<19&&y>-1){
			if(state[x*19+y]==color){
				count++;
				if(count==2) result=2;
				if(count==3) result=3;
				if(count==4) result=4;
				if(count==5) result=5;
			}
			else
				count=0;
			
			x+=1; y-=1;
		}
		count=0;
	}
	
	for(int j=19;j>-1;j--){
		int i=0;
		int x=i; int y=j;
		while(y>-1&&x<19){
			if(state[x*19+y]==color){
				count++;
				if(count==2) result=2;
				if(count==3) result=3;
				if(count==4) result=4;
				if(count==5) result=5;
			}
			else
				count=0;
			
			x+=1; y-=1;
		}
		count=0;
	}	

	return result;
}
///////////////////////////////////////////////////////////
int terminal_test(char state[400]){
	//terminal 아니면 -> return 0 (default)
	int test_result=0; 
	
	//user의 우승 -> return -1
	if(horizontal_win(state,player_color)==5||vertical_win(state,player_color)==5||Rdiagonal_win(state,player_color)==5||Ldiagonal_win(state,player_color)==5)
		return -1;
	
	//ai의 우승 -> return 1
	if(horizontal_win(state,ai_color)==5||vertical_win(state,ai_color)==5||Rdiagonal_win(state,ai_color)==5||Ldiagonal_win(state,ai_color)==5)
		return 1;

	//모든 자리가 다 찼을 때 -> return 2
	int flag=0;
	for(int i=0;i<20;i++){
		for(int j=0;j<20;j++){
			if(state[i*19+j]=='-')
				flag=1; //빈 자리가 존재
		}
	}	
	if(!flag)
		return 2;

	return test_result;
}
/////////////////estimate 기준들////////////////////////////
//1. 5-in-row를 만드는 action(+- INF*5)
int five_in_row(char state[400],int action){
	int count;
	int x=(action)/19; int y=(action)%19;
	//1)horizontal line
	
	count=0;
	for(int i=0;i<19;i++){
		for(int j=0;j<19;j++){
			if(state[i*19+j]==player_color){
				count++;
				if(count==5) return NINF*5; //5개연속 -> win,terminate 조건
			}
			else
				count=0;
		}
		count=0;
	}
	
	for(int i=-4;i<5;i++){
			if(state[x*19+(y+i)]==ai_color){
				count++;
				if(count==5)
					return INF*5;
			}
			else count=0;
	}
	//2)vertical line
	count=0;

	for(int j=0;j<19;j++){
		for(int i=0;i<19;i++){
			if(state[i*19+j]==player_color){
				count++;
				if(count==5) return NINF*5;
			}
			else
				count=0;
		}
		count=0;
	}
	for(int i=-4;i<5;i++){
			if(state[(x+i)*19+y]==ai_color){
				count++;
				if(count==5)
					return INF*5;
			}
			else count=0;
	}
	//3)Rdiagonal line
	count=0;
	for(int i=0;i<19;i++){
		int j=0;
		int x=i; int y=j;
		while(y<19&&x<19){
			if(state[x*19+y]==player_color){
				count++;
				if(count==5) return NINF*5;
			}
			else
				count=0;
			
			x+=1; y+=1;
		}
		count=0;
	}
	
	for(int j=1;j<19;j++){
		int i=0;
		int x=i; int y=j;
		while(x<19&&y<19){
			if(state[x*19+y]==player_color){
				count++;
				if(count==5) return NINF*5;
			}
			else
				count=0;
			
			x+=1; y+=1;
		}
		count=0;
	}	
	
	for(int i=-4;i<5;i++){
			if(state[(x+i)*19+(y+i)]==ai_color){
				count++;
				if(count==5)
					return INF*5;
			}
			else count=0;
	}
	//4)Ldiagonal line
	count=0;		
	for(int i=0;i<19;i++){
		int j=19;
		int x=i; int y=j;
		while(x<19&&y>-1){
			if(state[x*19+y]==player_color){
				count++;
				if(count==5) return NINF*5;
			}
			else
				count=0;
			
			x+=1; y-=1;
		}
		count=0;
	}
	
	for(int j=19;j>-1;j--){
		int i=0;
		int x=i; int y=j;
		while(y>-1&&x<19){
			if(state[x*19+y]==player_color){
				count++;
				if(count==5) return NINF*5;
			}
			else
				count=0;
			
			x+=1; y-=1;
		}
		count=0;
	}	
	
	for(int i=-4;i<5;i++){
			if(state[(x+i)*19+(y-i)]==ai_color){
				count++;
				if(count==5)
					return INF*5;
			}
			else count=0;
	}
	
	return 0;
}
//2. 한쪽 막힌 Player의 4-in-row를 차단하는 action. Player의 우승을 차단 (+5000)
int block_player_5(char state[400],int action){
	int count;
	int x=(action)/19; int y=(action)%19;
	
	//1)horizontal line
	count=0;
	if(state[x*19+(y+5)]==ai_color){
		for(int i=1;i<5;i++){
			if(state[x*19+(y+i)]==player_color){
				count++;
				if(count==4)
					return 5000;
			}
			else count=0;
		}
	}
	else if(state[x*19+(y-5)]==ai_color){
		for(int i=1;i<5;i++){
			if(state[x*19+(y-i)]==player_color){
				count++;
				if(count==4)
					return 5000;
			}
			else count=0;
		}
	}

	//2)vertical line
	count=0;
	if(state[(x+5)*19+y]==ai_color){
		for(int i=1;i<5;i++){
			if(state[(x+i)*19+y]==player_color){
				count++;
				if(count==4)
					return 5000;
			}
			else count=0;
		}
	}
	else if(state[(x-5)*19+y]==ai_color){
		for(int i=1;i<5;i++){
			if(state[(x-i)*19+y]==player_color){
				count++;
				if(count==4)
					return 5000;
			}
			else count=0;
		}
	}

	//3)Rdiagonal line
	count=0;
	if(state[(x+5)*19+(y+5)]==ai_color){
		for(int i=1;i<5;i++){
			if(state[(x+i)*19+(y+i)]==player_color){
				count++;
				if(count==4)
					return 5000;
			}
			else count=0;
		}
	}
	else if(state[(x-5)*19+(y-5)]==ai_color){
		for(int i=1;i<5;i++){
			if(state[(x-i)*19+(y-i)]==player_color){
				count++;
				if(count==4)
					return 5000;
			}
			else count=0;
		}
	}
	//4)Ldiagonal line
	count=0;
	if(state[(x+5)*19+(y-5)]==ai_color){
		for(int i=1;i<5;i++){
			if(state[(x+i)*19+(y-i)]==player_color){
				count++;
				if(count==4)
					return 5000;
			}
			else count=0;
		}
	}
	else if(state[(x-5)*19+(y+5)]==ai_color){
		for(int i=1;i<5;i++){
			if(state[(x-i)*19+(y+i)]==player_color){
				count++;
				if(count==4)
					return 5000;
			}
			else count=0;
		}
	}
	
	return 0;
}
//3. 양쪽이 뚫린 AI의 4-in-row를 만드는 action (+1000)
int make_open_4(char state[400],int action){
	int count=0;
	int x=(action)/19; int y=(action)%19;
	
	//horizontal line
	
	for(int i=-4;i<0;i++){
		if(state[x*19+(y+i)]=='-'){ //empty 만나면
			for(int j=1;j<5;j++){ //그 다음거부터 연속 4개 ai_color인지 검사
				if(state[x*19+(y+i+j)]==ai_color){
					count++;
					if(count==4 && state[x*19+(y+i+j+1)]=='-') //연속 4개고 다음이 empty
						return 1000;
					if(count>4){
						count=0;
						break;
					}
				}
				else {
					count=0;
					break;
				}
			}//for
		}//if
	}//for
	
	//vertical line

	for(int i=-4;i<0;i++){
		if(state[(x+i)*19+y]=='-'){ //empty 만나면
			for(int j=1;j<5;j++){ //그 다음거부터 연속 4개 ai_color인지 검사
				if(state[(x+i+j)*19+y]==ai_color){
					count++;
					if(count==4 && state[(x+i+j+1)*19+y]=='-') //연속 4개고 다음이 empty
						return 1000;
					if(count>4){
						count=0;
						break;
					}
				}
				else {
					count=0;
					break;
				}
			}//for
		}//if
	}//for
	
	//Rdiagonal line

	for(int i=-4;i<0;i++){
		if(state[(x+i)*19+(y+i)]=='-'){ //empty 만나면
			for(int j=1;j<5;j++){ //그 다음거부터 연속 4개 ai_color인지 검사
				if(state[(x+i+j)*19+(y+i+j)]==ai_color){
					count++;
					if(count==4 && state[(x+i+j+1)*19+(y+i+j+1)]=='-') //연속 4개고 다음이 empty
						return 1000;
					if(count>4){
						count=0;
						break;
					}
				}
				else {
					count=0;
					break;
				}
			}//for
		}//if
	}//for
	
	//Ldiagonal line

	for(int i=-4;i<0;i++){
		if(state[(x+i)*19+(y-i)]=='-'){ //empty 만나면
			for(int j=1;j<5;j++){ //그 다음거부터 연속 4개 ai_color인지 검사
				if(state[(x+i+j)*19+(y-i-j)]==ai_color){
					count++;
					if(count==4 && state[(x+i+j+1)*19+(y-i-j-1)]=='-') //연속 4개고 다음이 empty
						return 1000;
					if(count>4){
						count=0;
						break;
					}
				}
				else {
					count=0;
					break;
				}
			}//for
		}//if
	}//for


	return 0;
}
//4. Player의 양쪽이 뚫린 4row의 한쪽을 차단 (부질없지만 꼭 필요한 action임) (+500)
int block_player_4(char state[400],int action){
	int count;
	int x=(action)/19; int y=(action)%19;
//1)horizontal line
	count=0;
	if(state[x*19+(y+5)]=='-'){
		for(int i=1;i<5;i++){
			if(state[x*19+(y+i)]==player_color){
				count++;
				if(count==4)
					return 500;
			}
			else count=0;
		}
	}
	else if(state[x*19+(y-5)]=='-'){
		for(int i=1;i<5;i++){
			if(state[x*19+(y-i)]==player_color){
				count++;
				if(count==4)
					return 500;
			}
			else count=0;
		}
	}

	//2)vertical line
	count=0;
	if(state[(x+5)*19+y]=='-'){
		for(int i=1;i<5;i++){
			if(state[(x+i)*19+y]==player_color){
				count++;
				if(count==4)
					return 500;
			}
			else count=0;
		}
	}
	else if(state[(x-5)*19+y]=='-'){
		for(int i=1;i<5;i++){
			if(state[(x-i)*19+y]==player_color){
				count++;
				if(count==4)
					return 500;
			}
			else count=0;
		}
	}

	//3)Rdiagonal line
	count=0;
	if(state[(x+5)*19+(y+5)]=='-'){
		for(int i=1;i<5;i++){
			if(state[(x+i)*19+(y+i)]==player_color){
				count++;
				if(count==4)
					return 500;
			}
			else count=0;
		}
	}
	else if(state[(x-5)*19+(y-5)]=='-'){
		for(int i=1;i<5;i++){
			if(state[(x-i)*19+(y-i)]==player_color){
				count++;
				if(count==4)
					return 500;
			}
			else count=0;
		}
	}
	//4)Ldiagonal line
	count=0;
	if(state[(x+5)*19+(y-5)]=='-'){
		for(int i=1;i<5;i++){
			if(state[(x+i)*19+(y-i)]==player_color){
				count++;
				if(count==4)
					return 500;
			}
			else count=0;
		}
	}
	else if(state[(x-5)*19+(y+5)]=='-'){
		for(int i=1;i<5;i++){
			if(state[(x-i)*19+(y+i)]==player_color){
				count++;
				if(count==4)
					return 500;
			}
			else count=0;
		}
	}	
	return 0;
}
//5. Player의 open된 3row의 한쪽을 차단 (+300)
int block_player_3(char state[400],int action){
	int count;
	int x=(action)/19; int y=(action)%19;
	//1)horizontal line
	count=0;
	if(state[x*19+(y+5)]=='-'){
		for(int i=1;i<4;i++){
			if(state[x*19+(y+i)]==player_color){
				count++;
				if(count==3)
					return 300;
			}
			else count=0;
		}
	}
	else if(state[x*19+(y-5)]=='-'){
		for(int i=1;i<4;i++){
			if(state[x*19+(y-i)]==player_color){
				count++;
				if(count==3)
					return 300;
			}
			else count=0;
		}
	}

	//2)vertical line
	count=0;
	if(state[(x+5)*19+y]=='-'){
		for(int i=1;i<4;i++){
			if(state[(x+i)*19+y]==player_color){
				count++;
				if(count==3)
					return 300;
			}
			else count=0;
		}
	}
	else if(state[(x-5)*19+y]=='-'){
		for(int i=1;i<4;i++){
			if(state[(x-i)*19+y]==player_color){
				count++;
				if(count==3)
					return 300;
			}
			else count=0;
		}
	}

	//3)Rdiagonal line
	count=0;
	if(state[(x+5)*19+(y+5)]=='-'){
		for(int i=1;i<4;i++){
			if(state[(x+i)*19+(y+i)]==player_color){
				count++;
				if(count==3)
					return 300;
			}
			else count=0;
		}
	}
	else if(state[(x-5)*19+(y-5)]=='-'){
		for(int i=1;i<4;i++){
			if(state[(x-i)*19+(y-i)]==player_color){
				count++;
				if(count==3)
					return 300;
			}
			else count=0;
		}
	}
	//4)Ldiagonal line
	count=0;
	if(state[(x+5)*19+(y-5)]=='-'){
		for(int i=1;i<4;i++){
			if(state[(x+i)*19+(y-i)]==player_color){
				count++;
				if(count==3)
					return 300;
			}
			else count=0;
		}
	}
	else if(state[(x-5)*19+(y+5)]=='-'){
		for(int i=1;i<4;i++){
			if(state[(x-i)*19+(y+i)]==player_color){
				count++;
				if(count==3)
					return 300;
			}
			else count=0;
		}
	}	
	return 0;
}
//6. Player의 한쪽이 막힌 3row의 반대편을 차단(+100)
int block_closed_3(char state[400],int action){
	int count;
	int x=(action)/19; int y=(action)%19;
	//1)horizontal line
	count=0;
	if(state[x*19+(y+5)]==ai_color){
		for(int i=1;i<4;i++){
			if(state[x*19+(y+i)]==player_color){
				count++;
				if(count==3)
					return 100;
			}
			else count=0;
		}
	}
	else if(state[x*19+(y-5)]==ai_color){
		for(int i=1;i<4;i++){
			if(state[x*19+(y-i)]==player_color){
				count++;
				if(count==3)
					return 100;
			}
			else count=0;
		}
	}

	//2)vertical line
	count=0;
	if(state[(x+5)*19+y]==ai_color){
		for(int i=1;i<4;i++){
			if(state[(x+i)*19+y]==player_color){
				count++;
				if(count==3)
					return 100;
			}
			else count=0;
		}
	}
	else if(state[(x-5)*19+y]==ai_color){
		for(int i=1;i<4;i++){
			if(state[(x-i)*19+y]==player_color){
				count++;
				if(count==3)
					return 100;
			}
			else count=0;
		}
	}

	//3)Rdiagonal line
	count=0;
	if(state[(x+5)*19+(y+5)]==ai_color){
		for(int i=1;i<4;i++){
			if(state[(x+i)*19+(y+i)]==player_color){
				count++;
				if(count==3)
					return 100;
			}
			else count=0;
		}
	}
	else if(state[(x-5)*19+(y-5)]==ai_color){
		for(int i=1;i<4;i++){
			if(state[(x-i)*19+(y-i)]==player_color){
				count++;
				if(count==3)
					return 100;
			}
			else count=0;
		}
	}
	//4)Ldiagonal line
	count=0;
	if(state[(x+5)*19+(y-5)]==ai_color){
		for(int i=1;i<4;i++){
			if(state[(x+i)*19+(y-i)]==player_color){
				count++;
				if(count==3)
					return 100;
			}
			else count=0;
		}
	}
	else if(state[(x-5)*19+(y+5)]==ai_color){
		for(int i=1;i<4;i++){
			if(state[(x-i)*19+(y+i)]==player_color){
				count++;
				if(count==3)
					return 100;
			}
			else count=0;
		}
	}	
	return 0;
}

//7. AI의 open 3row를 생성 (+50)
int make_open_3(char state[400],int action){
	
	int x=(action)/19; int y=(action)%19;
	
	//horizontal line
	if(state[x*19+(y-2)]=='-'){
		if(state[x*19+(y-1)]=='-'){
			if(state[x*19+(y+1)]==ai_color&&state[x*19+(y+2)]==ai_color&&state[x*19+(y+3)]=='-') return 50;
		}
		else if(state[x*19+(y-1)]==ai_color){
			if(state[x*19+(y+1)]==ai_color&&state[x*19+(y+2)]=='-') return 50;
		}
	}
	else if(state[x*19+(y-2)]==ai_color){
		if(state[x*19+(y-3)]=='-'&&state[x*19+(y-1)]==ai_color&&state[x*19+(y+1)]=='-') return 50;
	}
	
	//Vertical line
	if(state[(x-2)*19+y]=='-'){
		if(state[(x-1)*19+y]=='-'){
			if(state[(x+1)*19+y]==ai_color&&state[(x+2)*19+y]==ai_color&&state[(x+3)*19+y]=='-') return 50;
		}
		else if(state[(x-1)*19+y]==ai_color){
			if(state[(x+1)*19+y]==ai_color&&state[(x+2)*19+y]=='-') return 50;
		}
	}
	else if(state[(x-2)*19+y]==ai_color){
		if(state[(x-3)*19+y]=='-'&&state[(x-1)*19+y]==ai_color&&state[(x+1)*19+y]=='-') return 50;
	}
	
	//Rdiagonal line
	if(state[(x-2)*19+(y-2)]=='-'){
		if(state[(x-1)*19+(y-1)]=='-'){
			if(state[(x+1)*19+(y+1)]==ai_color&&state[(x+2)*19+(y+2)]==ai_color&&state[(x+3)*19+(y+3)]=='-') return 50;
		}
		else if(state[(x-1)*19+(y-1)]==ai_color){
			if(state[(x+1)*19+(y+1)]==ai_color&&state[(x+2)*19+(y+2)]=='-') return 50;
		}
	}
	else if(state[(x-2)*19+(y-2)]==ai_color){
		if(state[(x-3)*19+(y-3)]=='-'&&state[(x-1)*19+(y-1)]==ai_color&&state[(x+1)*19+(y+1)]=='-') return 50;
	}

	//Ldiagonal line
	if(state[(x-2)*19+(y+2)]=='-'){
		if(state[(x-1)*19+(y+1)]=='-'){
			if(state[(x+1)*19+(y-1)]==ai_color&&state[(x+2)*19+(y-2)]==ai_color&&state[(x+3)*19+(y-3)]=='-') return 50;
		}
		else if(state[(x-1)*19+(y+1)]==ai_color){
			if(state[(x+1)*19+(y-1)]==ai_color&&state[(x+2)*19+(y-2)]=='-') return 50;
		}
	}
	else if(state[(x-2)*19+(y+2)]==ai_color){
		if(state[(x-3)*19+(y+3)]=='-'&&state[(x-1)*19+(y+1)]==ai_color&&state[(x+1)*19+(y-1)]=='-') return 50;
	}
	
	return 0;
}
//8. AI의 open 2row를 생성 (초반 move에 관여) (+10)
int make_open_2(char state[400],int action){
	int x=(action)/19; int y=(action)%19;
	//horizontal line
	if(state[x*19+(y-1)]=='-'){
		if(state[x*19+(y+1)]==ai_color&&state[x*19+(y+2)]=='-'){
			return 10;
		}
	}
	else if(state[x*19+(y-1)]==ai_color){
		if(state[x*19+(y-2)]=='-'&&state[x*19+(y+1)]=='-'){
			return 10;
		}
	}
	
	//Vertical line
	if(state[(x-1)*19+y]=='-'){
		if(state[(x+1)*19+y]==ai_color&&state[(x+2)*19+y]=='-'){
			return 10;
		}
	}
	else if(state[(x-1)*19+y]==ai_color){
		if(state[(x-2)*19+y]=='-'&&state[(x+1)*19+y]=='-'){
			return 10;
		}
	}
	
	//Rdiagonal line
	if(state[(x-1)*19+(y-1)]=='-'){
		if(state[(x+1)*19+(y+1)]==ai_color&&state[(x+2)*19+(y+2)]=='-'){
			return 10;
		}
	}
	else if(state[(x-1)*19+(y-1)]==ai_color){
		if(state[(x-2)*19+(y-2)]=='-'&&state[(x+1)*19+(y+1)]=='-'){
			return 10;
		}
	}
	
	//Ldiagonal line
	if(state[(x-1)*19+(y+1)]=='-'){
		if(state[(x+1)*19+(y-1)]==ai_color&&state[(x+2)*19+(y-2)]=='-'){
			return 10;
		}
	}
	else if(state[(x-1)*19+(y+1)]==ai_color){
		if(state[(x-2)*19+(y+2)]=='-'&&state[(x+1)*19+(y-1)]=='-'){
			return 10;
		}
	}
	

	return 0;
}
//////////////////////////////////////////////////////////
int estimate(int action,char state[400]){
	int util=0; //default
	int x=action/19; int y=action%19;
	
	int terminal=five_in_row(state,action);
	if(terminal!=0)
		return terminal;
	
	//모든 자리가 다 찼을 때 -> return 2
	int flag=0;
	for(int i=0;i<361;i++){
		if(state[i]=='-'){
			flag=1; //빈 자리가 존재
			break;
		}
	}	
	if(!flag) return -10; //빈 자리 없음
	
	util = block_player_5(state,action)+ make_open_4(state,action)+ block_player_4(state,action)
			+block_player_3(state,action)+ block_closed_3(state,action)+ make_open_3(state,action)+ make_open_2(state,action);

	return util;
}
///////////////////////////////////////////////////////////
int player_turn(char state[400]){
	cout<<'\n'<<"It's Your turn!"<<'\n';
	cout<<"Type the position where you want to put your stone."<<'\n';
	cout<<"example: (J,10)"<<'\n'<<'\n';
	char x; int y;
	
	int return_value=1;
	while(return_value!=2){
		cout<<"type position in form (Capital,integer) > ";
		cout.flush();
		return_value=scanf("(%c,%d)",&x,&y);
		while(getchar()!='\n');
		if(return_value==2)
			if(!(65<=x&&x<=83)||!(1<=y&&y<=19))
				return_value=1;			
			if(!action_isvalid(state,19*(x-65)+(y-1),1)){
				return_value=1;
			}
	}

	state[19*(x-65)+(y-1)]=player_color;
	return 19*(x-65)+(y-1);

}
///////////////////////////////////////////////////////////
int start(char state[400]){
	
	//시작화면
	cout<<"++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"<<'\n';
	cout<<"+++++++++++++++ ☆★☆★☆★☆★☆★☆★☆ +++++++++++++++"<<'\n';
	cout<<"+++++++++++++++ ★WELCOME TO GOMOKU GAME★ +++++++++++++++"<<'\n';
	cout<<"+++++++++++++++ ☆★☆★☆★☆★☆★☆★☆ +++++++++++++++"<<'\n';
	cout<<"++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"<<'\n';
	cout<<"++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"<<'\n';
	cout<<"+++++ Each turn, you have time limit : 20 seconds ++++++++"<<'\n';
	cout<<"++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"<<'\n';
	cout<<"++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"<<'\n';
	cout<<"++++++++++++++++ Game Starts in 2 seconds ++++++++++++++++"<<'\n';
	cout<<"+++++++++++++++++++++ Please Wait... +++++++++++++++++++++"<<'\n';
	cout<<"++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"<<'\n';
	
	cout.flush(); sleep(2);
	cout<<string(30,'\n'); 
	
	
	//choose player's stone color
	srand(time(NULL));
	int num = rand() % 2;
	
	if(num){ //player가 선공(black)
		player_color='B';
		ai_color='W';
		
		cout<<"You are Player 'Black'."<<'\n'<<'\n';
		show_state(state);

		cout<<'\n'<<"It's Your turn!"<<'\n';
		cout<<"Type the position where you want to put your stone."<<'\n';
		cout<<"example: (J,10)"<<'\n'<<'\n';
		char x; int y;
		
		int return_value=1;
		while(return_value!=2){
			cout<<"type position in form (Capital,integer) > ";
			cout.flush();
			return_value=scanf("(%c,%d)",&x,&y);
			
			while(getchar()!='\n');
			if(return_value==2)
			if(!(65<=x&&x<=83)||!(1<=y&&y<=19))
				return_value=1;			
		}
		
		state[19*(x-65)+(y-1)]=player_color;	
		return 19*(x-65)+(y-1);
	}
	
	else{ //player가 후공(white)
		player_color='W';
		ai_color='B';
		
		cout<<"You are Player 'White'. Now Computer is Playing..."<<'\n';
		cout.flush(); sleep(2);
		
		state[180]=ai_color; //put Black in the middle (J,10) initially
		show_state(state);

		int action= player_turn(state);
		return action;		
	}
	
}
///////////////////////////////////////////////////////////
int Iterative_Deepening_Search(char state[400]){
	int action;
	clock_t start_time,search_time;
	start_time=clock();
	
	for(int depth=1;depth<5;depth++){
		int cutoff=0;

		action=Find_action(state,depth,&cutoff);
		search_time=(clock()-start_time)/CLOCKS_PER_SEC;
		if(search_time>30){
			cout<<"\n\nAI Elapsed time : "<<search_time<<" seconds... \n"; 
			return -100; //시간 초과
		}
		
		if(cutoff==0) //cutoff not occured
			return action;
	}
	return action;
}
///////////////////////////////////////////////////////////
int Find_action(char state[400],int depth,int* cutoff){

	int best_value=NINF;
	srand(time(NULL));
	int action = rand() % 360;
	
	for(int act=0;act<361;act++){
		if(!action_isvalid(state,act,0)) //invalid action
			continue;
		//act에 ai 놨을때 다음엔 player 놔야됨
		state[act]=ai_color;
		int value=Alpha_Beta_Search(act,state,depth-1,NINF,INF,cutoff,1);
		state[act]='-';
		
		if(value>best_value){
			best_value=value;
			action=act;
		}
	}
	
	return action;	
	
}
///////////////////////////////////////////////////////////
int Alpha_Beta_Search(int action,char state[400],int depth,int a, int b, int* cutoff,int player){

	int est=estimate(action,state); //estimate function
	if(depth==0 || est==INF*5 || est==NINF*5 || est==-10){
		if(!depth){
			(*cutoff)=1; //cutoff occured
		}
		return est;
	}
	
	if(player==0){ //maximizing(ai playing)
		int value=NINF;
		int new_value;
		
		for(int act=0;act<361;act++){
			if(!action_isvalid(state,act,0)) //invalid action
				continue;
			(*cutoff)=0; 
			
			state[act]=ai_color;
			new_value=Alpha_Beta_Search(act,state,depth-1,a,b,cutoff,1);
			state[act]='-';
			
			value=max(new_value,value);
			if(value>=b) return value; 
			a=max(a,value);
		}
		return value;		
	}
	
	else{ //minimizing(player playing)
		int value=INF;
		int new_value;
		
		for(int act=0;act<361;act++){
			if(!action_isvalid(state,act,0)) //invalid action
				continue;
			(*cutoff)=0; 
			
			state[act]=player_color;
			new_value=Alpha_Beta_Search(act,state,depth-1,a,b,cutoff,0);
			state[act]='-';
			
			value=min(new_value,value);
			if(value<=a) return value; 
			b=min(b,value);
			
		}
		return value;	
	}
	
}

///////////////////////////////////////////////////////////
int main(void) {
	
	ios::sync_with_stdio(NULL); cin.tie(NULL);
	
//현재 state 초기화
	for(int i=0;i<361;i++){
		cur_state[i]='-';
	}

//game 시작
	auto tstart=chrono::steady_clock::now();
	int temp_act=start(cur_state);
	auto tend=chrono::steady_clock::now();
	int time=chrono::duration_cast<chrono::seconds>(tend-tstart).count();

	if(time>32){ //시간초과
		cur_state[temp_act]='-';
		cout<<"\n\nPlayer Elapsed time : "<<time<<" seconds... \n"; 
		cout<<"\nFinal State ↓\n";
		show_state(cur_state);
		cout<<"\n\nYou lose!\n\n";
		return 0;
	}
	show_state(cur_state); //첫 수 보여줌
	
	int terminal=0;
	int action=-1;
	
	while(true){
	//computer playing
		cout<<"Now Computer is Playing..."<<'\n';
		cout.flush(); sleep(2);
	//시간 재기 시작
		action=Iterative_Deepening_Search(cur_state);		
	//시간 재기 끝
		if(action==-100){
			terminal=-1;
			break; //player win
		}
		
		cur_state[action]=ai_color;
		terminal=terminal_test(cur_state);
		if(terminal!=0){
			break;
		}
		show_state(cur_state);
		
	//player playing
		//시간 재기 시작
		auto tstart=chrono::steady_clock::now();
		temp_act=player_turn(cur_state);
		auto tend=chrono::steady_clock::now();
		time=chrono::duration_cast<chrono::seconds>(tend-tstart).count();
		//시간 재기 끝
		if(time>30){
			cur_state[temp_act]='-';
			cout<<"\n\nPlayer Elapsed time : "<<time<<" seconds... \n"; 
			terminal=1;
			break; //ai win
		}
		
		terminal=terminal_test(cur_state);
		if(terminal!=0){
			break;
		}
			
		show_state(cur_state);
	}
	
	cout<<"\nFinal State ↓\n";
	show_state(cur_state);
	if(terminal==-1){//player win
		cout<<"\n\nYou win!\n\n";
	}
	else if(terminal==1){//ai win
		cout<<"\n\nYou lose!\n\n";
	}
	else{//더 이상 빈 자리가 없음->무승부
		cout<<"\n\nTie\n\n";
	}
	
	return 0;
} 


