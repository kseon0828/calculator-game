#include <wiringPi.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <lirc/lirc_client.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>

float tone[1] = {1000000/(523.3*2)};

#define LED_GREEN 	27
#define LED_RED 	26
#define BUZZER 		29

int result, correct=1, start = 0;
char inputResult[10];
int questNum =1, length = 0;
int nCnt = 0, cCnt = 0;

void randomOperate(){
	int operand1, operand2;
	char operator;

	srand(time(NULL));

	operand1 = rand()%90 + 10;
	operand2 = rand()%90 + 10;

	switch(rand()%3){
		case 0:
			operator = '+';
			result = operand1 + operand2;
			break;
		case 1:
			operator = '-';
			operand1 = (operand1 >= operand2)? operand1 : operand2;
			operand2 = (operand1 < operand2)? operand1 : operand2;
			result = operand1 - operand2;
			break;
		case 2:
			operator = '*';
			result = operand1 * operand2;
			break;
	}

	printf("Q%d) %d %c %d = ",questNum++, operand1, operator, operand2);
	fflush(stdout);
}

void skDelay(unsigned int t){
	unsigned int cur = micros();

	while( micros()<(cur+ t));
}

void *sound(void *argumentPointer)
{	
	unsigned int now;
	int arr[] = {0};
	int arrSize = sizeof(arr)/sizeof(int);

	while(1){
		if(correct==0){
			for(int i=0 ; i<arrSize; i++){
				now = millis();

				while(millis()<(now + 500)){
					digitalWrite(BUZZER, 1);
					skDelay(tone[arr[i]]);
					digitalWrite(BUZZER, 0);
					skDelay(tone[arr[i]]);
				}
			}
			
		}
	}
}

void ledOn(int ledNum){
	if(ledNum == 1) {
		digitalWrite(LED_GREEN, 1);
		digitalWrite(LED_RED, 0);
		correct = 1;
	}
	else {
		digitalWrite(LED_GREEN, 0);
		digitalWrite(LED_RED, 1);
		start = 1;
		correct = 0;
	}
}

void checkAnswer(int inputNum){
	nCnt++;
	if(result == inputNum){
		printf("\ncorrect!\n\n");
		cCnt++;
		ledOn(1);
	} else{
		printf("\nwrong :(\nCorrect Answer : %d, Your Answer : %d\n\n", result, inputNum);
		ledOn(2);
	}
}


int main(){
	struct lirc_config *config;
	pthread_t threadID1, threadID2;

	int buttonTimer = millis();
	char *code;

	if(wiringPiSetup()==-1) return -1;

	pinMode(LED_GREEN, OUTPUT);
	pinMode(LED_RED, OUTPUT);
	pinMode(BUZZER, OUTPUT);

	pthread_create(&threadID1, NULL, sound, NULL);

	if(lirc_init("lirc", 1)==-1)
		exit(EXIT_FAILURE);

	while(1){
		if(lirc_readconfig(NULL, &config, NULL) == 0){
			randomOperate();
			while(lirc_nextcode(&code) ==0){
				if(code ==NULL)
					continue;

				if(millis() - buttonTimer > 600){										
					if(strstr(code, "KEY_1")){
						printf("1");
						fflush(stdout);
						inputResult[length++] = '1';
						buttonTimer = millis();					
					} else if(strstr(code, "KEY_2")){
						printf("2");
						fflush(stdout);
						inputResult[length++] = '2';
						buttonTimer = millis();				
					} else if(strstr(code, "KEY_3")){
						printf("3");
						fflush(stdout);
						inputResult[length++] = '3';
						buttonTimer = millis();				
					} else if(strstr(code, "KEY_4")){
						printf("4");
						fflush(stdout);
						inputResult[length++] = '4';
						buttonTimer = millis();				
					} else if(strstr(code, "KEY_5")){
						printf("5");
						fflush(stdout);
						inputResult[length++] = '5';
						buttonTimer = millis();				
					} else if(strstr(code, "KEY_6")){
						printf("6");
						fflush(stdout);
						inputResult[length++] = '6';
						buttonTimer = millis();				
					} else if(strstr(code, "KEY_7")){
						printf("7");
						fflush(stdout);
						inputResult[length++] = '7';
						buttonTimer = millis();				
					} else if(strstr(code, "KEY_8")){
						printf("8");
						fflush(stdout);
						inputResult[length++] = '8';
						buttonTimer = millis();				
					} else if(strstr(code, "KEY_9")){
						printf("9");
						fflush(stdout);
						inputResult[length++] = '9';
						buttonTimer = millis();				
					} else if(strstr(code, "KEY_0")){
						printf("0");
						fflush(stdout);
						inputResult[length++] = '0';
						buttonTimer = millis();
					} else if(strstr(code, "KEY_ENTER")){
						checkAnswer(atoi(inputResult));
												
						for(int i=0; i<length; i++){
							inputResult[i] = '\0';
						}
						length = 0;
						buttonTimer = millis();
						break;
					} else if(strstr(code, "KEY_DOWN")){
						printf("STOP\n\n");

						printf("%d/%d(right/total)\nScore : %f\n\n", cCnt, nCnt, cCnt/(float)nCnt);

						digitalWrite(LED_GREEN, 0);
						digitalWrite(LED_RED, 0);
						
						pthread_join(threadID1, NULL);

						free(code);
						lirc_freeconfig(config);
						lirc_deinit();
						exit(EXIT_SUCCESS);
					}
				}
				free(code);
			}
			lirc_freeconfig(config);
		}
	}
	lirc_deinit();
	exit(EXIT_SUCCESS);

	return 0;
}
