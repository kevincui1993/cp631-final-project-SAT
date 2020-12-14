#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>


int evaluateExpression(char* expression,int leftLimit, int rightLimit, int inputs[], int inputLength){
	/*
	Inputs:

	char* expression: This is a string that contains the circuit to be evaluated, the assumed form
	wraps a paranthteses around each gate, uses & for and gates, | for or gates and ! for not gates
	int leftLimit: This is where in the string that we begin the current evaluation
	int rightLimit: This is as far as we are meant to go as we incrment the index
	int input[]: This contains the values of the inputs gates
	int inputLength: This is the number of values in the input[] array

	Outputs:
	This function outputs either a 1 for a true output or a 0 for a false output

	*/
	int left = 0;
	int right = 0;
	int bracketCounter = 0;
	char leftArr[] = "";
	char rightArr[] = "";
	char operand;

	int index = leftLimit;
	if(index >= rightLimit || expression[index] == ')'){
		return 0;
	}
	//Their are 3 scenarios
	//1. the first expression is a bracket
	if(expression[index] == '('){
		//This branch finds the associated right bracket, it then evaluates the inner expression

		//If the first char is a left bracket we advance until the matching right bracket is found
		bracketCounter++;
		int leftIndex = index + 1;
		index++;
		while(bracketCounter > 0){
			if(expression[index] == '('){
				bracketCounter++;
			}
			else if(expression[index] == ')'){
				bracketCounter--;
			}
			index++;
		}
		//Here the index will point at the next operator
		operand = expression[index];
		//Increment the index to advance to the start of the right expression
		//printf("bracket Eval : left: %3d and right %3d\n",leftIndex,rightIndex);
		left  = evaluateExpression(expression,leftIndex,index - 1,inputs,inputLength);

		index++;
		int rightIndex = index;
		//This handles the scenario where the only portion to be evaluated is within a single set of parantheses
		if(index >= rightLimit || expression[rightIndex] ==  ')'){
			return left;
		}

		right = evaluateExpression(expression,rightIndex,rightLimit,inputs,inputLength);
		//Evaluate the left and right values
		if(operand == '&'){
			return left && right;
		}
		else if(operand == '|'){
			return left || right;
		}
	}
	//2. The first expression is a number
	else if(isdigit(expression[index])){
		while(index < rightLimit && isdigit(expression[index])){
			strncat(leftArr, &expression[index], 1); 
			index++;
		}
		if(index >= rightLimit || expression[index] == ')'){
			//printf("number %3d\n",atoi(leftArr));
			return inputs[atoi(leftArr)];
		}
		else{
			left = inputs[atoi(leftArr)];
			operand = expression[index];
			right = evaluateExpression(expression,index + 1,rightLimit,inputs,inputLength);
			//printf("left: %3d and right %3d\n",atoi(leftArr),right);
			

			if(operand == '&'){
			return left && right;
			}
			else if(operand == '|'){
				return left || right;
			}
		}
		
	}
	//3. The first expression is a negation
	else if(expression[index] == '!'){
		//their are two scenarios here
		//1. negating an expression, this is simple just negate the entire thing
		if(expression[index+1] == '('){
			return !evaluateExpression(expression,index + 1,rightLimit,inputs,inputLength);
		}
		//negating a number
		else{
			
			index++;
			//Acess all sequential numbers and store them in a c string
			while(index < rightLimit && isdigit(expression[index])){
				strncat(leftArr, &expression[index], 1); 
				index++;
			}
			if(index >= rightLimit || expression[index] == ')'){
				return !inputs[atoi(leftArr)];
			}
			else{
				//perform the negation
				left = !inputs[atoi(leftArr)];
				operand = expression[index];
				right = evaluateExpression(expression,index + 1,rightLimit,inputs,inputLength);				

				if(operand == '&'){
				return left && right;
				}
				else if(operand == '|'){
					return left || right;
				}
			}
		}
	}
	else{
		printf("something weird happend at index in the logical expression reader %3d\n",index);
		printf("Char is %c\n",expression[index]);
	}
	
}

void resetArr(int* arr, int length){
	int i = 0;
	for(i = 0 ; i < length; i++){
		arr[i] = 0;
	}
}


void runAllTests(){

	char* expression;
	int expressionLength;


	//Test 1
	expression = "0&1";
	expressionLength = 3;
	int inputs[100] = {0};
	inputs[0] = 1;
	inputs[1] = 1;

	printf("Test 1 value : %3d expected %3d\n",evaluateExpression(expression,0,expressionLength,inputs,100),1);

	resetArr(inputs,100);

	//Test 2
	expression = "0|1";
	expressionLength = 3;
	inputs[0] = 0;
	inputs[1] = 1;

	printf("Test 2 value : %3d expected %3d\n",evaluateExpression(expression,0,expressionLength,inputs,100),1);

	resetArr(inputs,100);

	//Test 3
	expression = "0|1";
	expressionLength = 3;
	inputs[0] = 0;
	inputs[1] = 0;

	printf("Test 3 value : %3d expected %3d\n",evaluateExpression(expression,0,expressionLength,inputs,100),0);

	resetArr(inputs,100);


	//Test 4
	expression = "(0|1)&(1&2)";
	expressionLength = 11;
	inputs[0] = 0;
	inputs[1] = 1;
	inputs[2] = 1;

	printf("Test 4 value : %3d expected %3d\n",evaluateExpression(expression,0,expressionLength,inputs,100),1);

	resetArr(inputs,100);


	//Test 5
	expression = "(0|1)&(1&2)";
	expressionLength = 11;
	inputs[0] = 0;
	inputs[1] = 1;
	inputs[2] = 0;

	printf("Test 5 value : %3d expected %3d\n",evaluateExpression(expression,0,expressionLength,inputs,100),0);

	resetArr(inputs,100);


	//Test 6
	expression = "(0|1)&!(1&2)";
	expressionLength = 11;
	inputs[0] = 1;
	inputs[1] = 0;
	inputs[2] = 0;

	printf("Test 6 value : %3d expected %3d\n",evaluateExpression(expression,0,expressionLength,inputs,100),1);

	resetArr(inputs,100);


	//Test 7
	expression = "(0|1)&!(1&2)";
	expressionLength = 11;
	inputs[0] = 1;
	inputs[1] = 1;
	inputs[2] = 1;

	printf("Test 7 value : %3d expected %3d\n",evaluateExpression(expression,0,expressionLength,inputs,100),0);

	resetArr(inputs,100);

	//Test 8
	expression = "(((0&1)&3)|!(1|2))&((4|5)&(!6|(7&8)))";
	expressionLength = 37;
	inputs[0] = 1;
	inputs[3] = 1;
	inputs[4] = 1;
	inputs[7] = 1;

	printf("Test 8 value : %3d expected %3d\n",evaluateExpression(expression,0,expressionLength,inputs,100),1);

	resetArr(inputs,100);


	//Test 9
	expression = "(((0&1)&3)|!(1|2))&((4|5)&(!6|(7&8)))";
	expressionLength = 37;
	inputs[0] = 1;
	inputs[3] = 1;
	inputs[4] = 0;
	inputs[7] = 1;

	printf("Test 9 value : %3d expected %3d\n",evaluateExpression(expression,0,expressionLength,inputs,100),0);

	resetArr(inputs,100);

	//Test 10
	expression = "(((0&1)&3)|!(1|2))&((4|5)&(!6|(7&8)))";
	expressionLength = 37;

	printf("Test 10 value : %3d expected %3d\n",evaluateExpression(expression,0,expressionLength,inputs,100),0);

	resetArr(inputs,100);


	//The above functions were created to test the parser
	//These circuits are intended to be used in the project

	expression = "((((0&1)|!2)|(3&4))&!((5|6)&7))";

	expressionLength = 30;
	inputs[3] = 1;
	inputs[4] = 1;
	inputs[5] = 1;
	inputs[11] = 1;
	inputs[12] = 1;
	inputs[15] = 1;


	printf("Circuit 1 value : %3d expected %3d\n",evaluateExpression(expression,0,expressionLength,inputs,100),1);

	resetArr(inputs,100);


	expression = "(((8&(9&10))|(11&12))&((!13|!14)&15))";
	expressionLength = 37;
	inputs[3] = 1;
	inputs[4] = 1;
	inputs[5] = 1;
	inputs[11] = 1;
	inputs[12] = 1;
	inputs[15] = 1;


	printf("Circuit 1 value : %3d expected %3d\n",evaluateExpression(expression,0,expressionLength,inputs,100),1);

	resetArr(inputs,100);


	//Circuit 2 Test 1 16 inputs
	expression = "((!0|((1&2)&(3&4))&5)|(!6|((7|8)&9)))&(((!10|11)|(12&13))&((12&13)|(14&15)))";
	expressionLength = 76;
	inputs[14] = 1;
	inputs[15] = 1;

	printf("Circuit 2 value : %3d expected %3d\n",evaluateExpression(expression,0,expressionLength,inputs,100),1);

	resetArr(inputs,100);


	//Circuit 2 Test 2 16 inputs

	printf("Circuit 2 value : %3d expected %3d\n",evaluateExpression(expression,0,expressionLength,inputs,100),0);

	resetArr(inputs,100);

	//Circuit 2 Test 3 16 inputs
	inputs[0] = 1;
	inputs[3] = 1;
	inputs[5] = 1;
	inputs[8] = 1;
	inputs[9] = 1;
	inputs[10] = 1;
	inputs[15] = 1;

	printf("Circuit 2 value : %3d expected %3d\n",evaluateExpression(expression,0,expressionLength,inputs,100),0);

	resetArr(inputs,100);


	//Circuit 3 Test 1 16 inputs
	expression = "!((((0|1)|(2|3))|((4|5)|(6|7)))|(((8|9)|(10|11))|((12|13)|(14|15))))";
	expressionLength = 68;
	inputs[14] = 1;
	inputs[15] = 1;

	printf("Circuit 3 value : %3d expected %3d\n",evaluateExpression(expression,0,expressionLength,inputs,100),0);

	resetArr(inputs,100);


	//Circuit 3 Test 2 16 inputs

	printf("Circuit 3 value : %3d expected %3d\n",evaluateExpression(expression,0,expressionLength,inputs,100),1);

	resetArr(inputs,100);

	//Circuit 3 Test 3 16 inputs
	inputs[0] = 1;
	inputs[3] = 1;
	inputs[5] = 1;
	inputs[8] = 1;
	inputs[9] = 1;
	inputs[10] = 1;
	inputs[15] = 1;

	printf("Circuit 3 value : %3d expected %3d\n",evaluateExpression(expression,0,expressionLength,inputs,100),0);

	resetArr(inputs,100);


	//Circuit 4 Test 1 15 inputs
	expression = "(((6&7)&!8)&((((0|1)&!2)|((1&2)|((3|4)&5)))&((((11|12)&13)|(9&10))&!14)))";
	expressionLength = 74;
	inputs[14] = 1;
	inputs[15] = 1;

	printf("Circuit 4 value : %3d expected %3d\n",evaluateExpression(expression,0,expressionLength,inputs,100),0);

	resetArr(inputs,100);


	//Circuit 4 Test 2 15 inputs

	printf("Circuit 4 value : %3d expected %3d\n",evaluateExpression(expression,0,expressionLength,inputs,100),0);

	resetArr(inputs,100);

	//Circuit 4 Test 3 15 inputs
	inputs[0] = 1;
	inputs[3] = 1;
	inputs[5] = 1;
	inputs[8] = 1;
	inputs[9] = 1;
	inputs[10] = 1;
	inputs[15] = 1;

	printf("Circuit 4 value : %3d expected %3d\n",evaluateExpression(expression,0,expressionLength,inputs,100),0);

	resetArr(inputs,100);


	//Circuit 5 Test 1 15 inputs
	expression = "((((0&1)&(2|3))|((4&5)|(6|7)))&(((8&9)&(10|11))&((12&13)|!14)))&!((((0&1)&(2|3))|((4&5)|(6|7)))&(((8&9)&(10|11))&((12&13)|!14)))";
	expressionLength = 128;
	inputs[14] = 1;
	inputs[15] = 1;

	printf("Circuit 5 value : %3d expected %3d\n",evaluateExpression(expression,0,expressionLength,inputs,100),0);

	resetArr(inputs,100);


	//Circuit 5 Test 2 16 inputs

	printf("Circuit 5 value : %3d expected %3d\n",evaluateExpression(expression,0,expressionLength,inputs,100),0);

	resetArr(inputs,100);

	//Circuit 5 Test 3 16 inputs
	inputs[0] = 1;
	inputs[3] = 1;
	inputs[5] = 1;
	inputs[8] = 1;
	inputs[9] = 1;
	inputs[10] = 1;
	inputs[15] = 1;

	printf("Circuit 5 value : %3d expected %3d\n",evaluateExpression(expression,0,expressionLength,inputs,100),0);

	resetArr(inputs,100);


	//Circuit 6 Test 1 15 inputs
	expression = "((((((((((((((0&1)|2)&3)|4)&5)|6)&7)|8)&9)|10)&!11)|12)&13)|14)";
	expressionLength = 63;
	inputs[6] = 1;
	inputs[7] = 1;


	printf("Circuit 6 value : %3d expected %3d\n",evaluateExpression(expression,0,expressionLength,inputs,100),0);
	resetArr(inputs,100);


	//Circuit 6 Test 2 16 inputs

	printf("Circuit 6 value : %3d expected %3d\n",evaluateExpression(expression,0,expressionLength,inputs,100),0);

	resetArr(inputs,100);

	//Circuit 6 Test 3 16 inputs
	inputs[14] = 1;

	printf("Circuit 6 value : %3d expected %3d\n",evaluateExpression(expression,0,expressionLength,inputs,100),1);

	resetArr(inputs,100);

	expression = "((!(!0&1)|!(2&15))&!((!4|5)|!(6|7)))&!(((!8&9)&(10&11))&(!(12|13)&((14&3)|!16)))";
	expressionLength = 80;
	inputs[6] = 1;
	inputs[7] = 1;
	inputs[11] = 1;
	inputs[13] = 1;


	printf("Circuit 7 value : %3d expected %3d\n",evaluateExpression(expression,0,expressionLength,inputs,100),0);
	resetArr(inputs,100);


	//Circuit 7 Test 2 16 inputs

	printf("Circuit 7 value : %3d expected %3d\n",evaluateExpression(expression,0,expressionLength,inputs,100),0);

	resetArr(inputs,100);

	//Circuit 7 Test 3 16 inputs
	inputs[14] = 1;

	printf("Circuit 7 value : %3d expected %3d\n",evaluateExpression(expression,0,expressionLength,inputs,100),0);

	resetArr(inputs,100);


	//Circuit 8 Test 1 18 inputs
	expression = "(15&(16&17)&((6&7)&!8)&((((0|1)&!2)|((1&2)|((3|4)&5)))&((((11|12)&13)|(9&10))&!14)))";
	expressionLength = 84;
	inputs[6] = 1;
	inputs[7] = 1;
	inputs[11] = 1;
	inputs[13] = 1;


	printf("Circuit 8 value : %3d expected %3d\n",evaluateExpression(expression,0,expressionLength,inputs,100),0);
	resetArr(inputs,100);


	//Circuit 8 Test 2 18 inputs

	printf("Circuit 8 value : %3d expected %3d\n",evaluateExpression(expression,0,expressionLength,inputs,100),0);

	resetArr(inputs,100);

	//Circuit 8 Test 3 18 inputs
	inputs[0] = 1;
	inputs[16] = 1;
	inputs[17] = 1;
	inputs[15] = 1;
	inputs[6] = 1;
	inputs[7] = 1;
	inputs[9] = 1;
	inputs[10] = 1;
	//100000110110000111
	printf("Circuit 8 value : %3d expected %3d\n",evaluateExpression(expression,0,expressionLength,inputs,100),1);

	resetArr(inputs,100);


	//Circuit 9 no solution
	expression = "(!((0&1)&2)|(((3|!4)&5)&(6|7)))&(((8&9)|!(10&11))&(((!12|13)&((14&15)&(16&17)))))&!(!((0&1)&2)|(((3|!4)&5)&(6|7)))&(((8&9)|!(10&11))&(((!12|13)&((14&15)&(16&17)))))";
	expressionLength = 164;
	inputs[6] = 1;
	inputs[7] = 1;
	inputs[11] = 1;
	inputs[13] = 1;


	printf("Circuit 9 value : %3d expected %3d\n",evaluateExpression(expression,0,expressionLength,inputs,100),0);
	resetArr(inputs,100);


	//Circuit 9 Test 2 16 inputs

	printf("Circuit 9 value : %3d expected %3d\n",evaluateExpression(expression,0,expressionLength,inputs,100),0);

	resetArr(inputs,100);

	//Circuit 9 Test 3 16 inputs
	inputs[14] = 1;

	printf("Circuit 9 value : %3d expected %3d\n",evaluateExpression(expression,0,expressionLength,inputs,100),0);

	resetArr(inputs,100);


	//Circuit 10 Test 1
	expression = "(((0|!1)&(2&(3|!4))|!(5&6))|(((7&!8)|9)&(10|11)))|((((12|13)&!14)&(15&(16|17)))|!18)";
	expressionLength = 84;
	inputs[6] = 1;
	inputs[7] = 1;
	inputs[11] = 1;
	inputs[13] = 1;


	printf("Circuit 10 value : %3d expected %3d\n",evaluateExpression(expression,0,expressionLength,inputs,100),1);
	resetArr(inputs,100);


	//Circuit 10 Test 2 16 inputs

	printf("Circuit 10 value : %3d expected %3d\n",evaluateExpression(expression,0,expressionLength,inputs,100),1);

	resetArr(inputs,100);

	//Circuit 10 Test 3 16 inputs
	inputs[14] = 1;

	printf("Circuit 10 value : %3d expected %3d\n",evaluateExpression(expression,0,expressionLength,inputs,100),1);

	resetArr(inputs,100);

	//Circuit 11 Test 1
	expression = "((((0&1)|!2)&((3|!4)|(5&6)))&(!7|!8)&!(9|10))&((11|12)&(13|14))|((15|16)&(17|18))";
	expressionLength = 81;
	inputs[6] = 1;
	inputs[7] = 1;
	inputs[11] = 1;
	inputs[13] = 1;


	printf("Circuit 11 value : %3d expected %3d\n",evaluateExpression(expression,0,expressionLength,inputs,100),1);
	resetArr(inputs,100);


	//Circuit 11 Test 2 16 inputs

	printf("Circuit 11 value : %3d expected %3d\n",evaluateExpression(expression,0,expressionLength,inputs,100),0);

	resetArr(inputs,100);

	//Circuit 11 Test 3 16 inputs
	inputs[14] = 1;

	printf("Circuit 11 value : %3d expected %3d\n",evaluateExpression(expression,0,expressionLength,inputs,100),0);

	resetArr(inputs,100);

	//Circuit 12 no solution
	expression = "(((((0&1)|!2)&((3|!4)|(5&6)))&(!7|!8)&!(9|10))&((11|12)&(13|14))|((15|16)&(17|18)))&!(((((0&1)|!2)&((3|!4)|(5&6)))&(!7|!8)&!(9|10))&((11|12)&(13|14))|((15|16)&(17|18)))";
	expressionLength = 168;
	inputs[6] = 1;
	inputs[7] = 1;
	inputs[11] = 1;
	inputs[13] = 1;


	printf("Circuit 12 value : %3d expected %3d\n",evaluateExpression(expression,0,expressionLength,inputs,100),0);
	resetArr(inputs,100);


	//Circuit 12 Test 2 16 inputs

	printf("Circuit 12 value : %3d expected %3d\n",evaluateExpression(expression,0,expressionLength,inputs,100),0);

	resetArr(inputs,100);

	//Circuit 12 Test 3 16 inputs
	inputs[14] = 1;

	printf("Circuit 12 value : %3d expected %3d\n",evaluateExpression(expression,0,expressionLength,inputs,100),0);

	resetArr(inputs,100);
}

/*
int main(){
	runAllTests();
	return 0;
}
*/