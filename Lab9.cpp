#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <ctime>
#include <chrono>

using namespace std;

void stream_cleaning() {
	cin.clear();
	cin.ignore(cin.rdbuf()->in_avail());
}

char* int_to_char(int i) {
	char* str = new char[15];
	str[0] = '\0';
	while (i > 0) {
		for (int i = 14; i > 0; i--)//сдвиг вправо
			str[i] = str[i - 1];
		str[0] = (unsigned char)(48 + i % 10);
		i = i / 10;
	}
	return str;
}

int insert_variable(char*& expression, int i, int value) {
	char* c_value = int_to_char(value);
	for (int i = 14; i > 0; i--)
		c_value[i] = c_value[i - 1];
	c_value[0] = '(';//добавление скобок для разделения однозначных и многозначных чисел
	for (int i = 0; i < 15; i++)
		if (c_value[i] == '\0') {
			c_value[i] = ')';
			c_value[i + 1] = '\0';
			break;
		}
	int c_value_lenght = strlen(c_value);
	char* c_value_ptr = c_value;
	for (int j = i; j < i + c_value_lenght; j++) {//вставка значения на место переменной
		if (j != i)
			for (int k = 99; k > j; k--)
				expression[k] = expression[k - 1];
		expression[j] = *c_value_ptr;
		c_value_ptr++;
	}
	delete[] c_value;
	return c_value_lenght - 1;
}

void filling_variables(char*& expression) {
	bool exit = 1;
	for (int i = 0; i < 100; i++) {//поиск переменных
		if ((unsigned char)expression[i] > 96 and (unsigned char)expression[i] < 123)//обычные буквы англ. алфавита
			exit = 0;
		if (expression[i] == '\0')
			break;
	}
	char name;
	int value;
	while (!exit) {
		exit = 1;
		cout << "Введите имя переменной и ее значение через пробел\n";
		cin >> name;
		if (cin >> value);//если нет пробела, ввод переменной, если есть, считывание пробела, потом ввод переменной
		else {
			cin.clear();
			cin.get();
			cin >> value;
		}
		stream_cleaning();

		for (int i = 0; i < 100; i++) {//поиск и вставка всех переменных с именем name
			if (expression[i] == '\0')
				break;

			if (expression[i] == name)
				i += insert_variable(expression, i, value);

			if ((unsigned char)expression[i] > 96 and (unsigned char)expression[i] < 123 and expression[i] != name)
				exit = 0;
		}
		cout << expression << endl;
	}
}

bool check_reverse_polish_notation(char* expression) {
	int counter_operands = 0, counter_operations = 0;
	unsigned char c;//перевод в беззнаковый символ для более простого сравнения с таблицей аски
	for (int i = 0; i < 100; i++) {
		c = expression[i];
		if (c >= 48 and c <= 57 or c >= 97 and c <= 122)//операнды
			counter_operands++;
		else if (c == 42 or c == 43 or c == 45 or c == 47)//операции
			counter_operations++;
		else if (c != '\0')
			return 0;
		else
			break;
		if (counter_operands <= counter_operations)//достаточно проверять каждую итерацию количество операндов и операций
			return 0;
	}
	if (counter_operands != counter_operations + 1)
		return 0;
	else
		return 1;
}

bool check_polish_notation(char* expression) {
	int counter_operands = 0, counter_operations = 0;
	int i = 0;
	while (expression[i] != '\0')//проверяю так же, как обратную польскую, но с конца
		i++;
	unsigned char c;
	for (int j = i - 1; j >= 0; j--) {
		c = expression[j];
		if (c >= 48 and c <= 57 or c >= 97 and c <= 122)//операнды
			counter_operands++;
		else if (c == 42 or c == 43 or c == 45 or c == 47)//операции
			counter_operations++;
		else
			return 0;
		if (counter_operands <= counter_operations)
			return 0;
	}
	if (counter_operands != counter_operations + 1)
		return 0;
	else
		return 1;
}

bool service_check_simple_expression(char*& expression, bool first) {
	bool b = 0;
	int counter_operands = 0, counter_operations = 0;
	unsigned char c = *expression;
	while (!(c == ')' or c == '\0')) {
		b = 1;
		if (c >= 48 and c <= 57 or c >= 97 and c <= 122)//цифры и буквы англ. алфавита
			counter_operands++;
		else if ((c == 42 or c == 43 or c == 45 or c == 47) and counter_operands)//операции
			counter_operations++;
		else if (c != '(')
			return 0;

		if (c == '(') {
			if (!service_check_simple_expression(++expression, 0))//открытие скобок означает рекурсивный вызов функции
				return 0;
			counter_operands++;//то, что в скобках, тоже операнд
			c = *expression;//рекурсия сама перейдет к нужному символу
		}
		else {
			expression++;
			c = *expression;//переход к следующему символу
		}
		if (counter_operands >= 2)//проверка количеста операндов и операций
			if (counter_operations != 1)
				return 0;
			else {
				counter_operands--;
				counter_operations--;
			}
		if (counter_operations >= 2)
			return 0;
	}
	if (!first)//если будет лишний символ ')', он не удалится
		if (c != '\0')
			expression++;
	return b;
}

bool check_simple_expression(char* expression) {//для того, чтобы не создавать в main указатель на выражение, используется эта функция
	if (service_check_simple_expression(expression, 1) and *expression == '\0')
		return 1;
	else
		return 0;
}

template<class T>
struct Stack {
	T info = 0;
	Stack* previous = nullptr;
};

int char_to_int(char*& str) {//перевод из char(число слева направо) в int
	int in = 0;
	unsigned char c = *str;
	while (c >= 48 and c <= 57) {
		in *= 10;
		in += c - 48;
		str++;
		c = *str;
	}
	return in;
}

int calculating_reverse_polish_notation(char* expression) {
	Stack<int>* head = nullptr;
	Stack<int>* new_s = nullptr;
	int summ = 0;
	unsigned char c;
	for (c = *expression; c != '\0'; c = *(++expression)) {
		if (c >= 48 and c <= 57 or c == '(') {//операнды
			new_s = new Stack<int>;//создание нового объекта стека для операнда
			new_s->previous = head;
			head = new_s;
			if (c != '(')//в скобках числа больше 9
				head->info = c - 48;
			else {
				head->info = char_to_int(++expression);
			}
		}
		else {//операторы
			int operand1 = head->info;//достаем 2 последних операнда
			new_s = head->previous;
			delete head;//соответственно, 1 удаляем
			head = new_s;
			int operand2 = head->info;
			switch (c) {
			case '*':
				summ = operand2 * operand1;
				break;
			case '+':
				summ = operand2 + operand1;
				break;
			case '-':
				summ = operand2 - operand1;
				break;
			case '/':
				summ = operand2 / operand1;
				break;
			}
			head->info = summ;//помещаем результат на место 2 операнда
		}
	}
	delete head;
	return summ;
}

int reverse_char_to_int(char*& str, int& i) {//перевод из char(число справа налево) в int
	int in = 0, counter = 0;
	unsigned char c = *str;
	while (c >= 48 and c <= 57) {
		in += (c - 48) * pow(10, counter);
		str--;
		c = *str;
		i--;
		counter++;
	}
	return in;
}

int calculating_polish_notation(char* expression) {
	int i = strlen(expression);
	while (*expression != '\0')//переход в конец выражения
		expression++;
	Stack<int>* head = nullptr;
	Stack<int>* new_s = nullptr;
	int summ = 0;
	unsigned char c;
	for (c = *(--expression); i > 0; c = *(--expression)) {//вычисление справа налево
		if (c >= 48 and c <= 57 or c == ')') {//операнды
			new_s = new Stack<int>;
			new_s->previous = head;
			head = new_s;
			if (c != ')')
				head->info = c - 48;
			else {
				head->info = reverse_char_to_int(--expression, --i);
			}
		}
		else {//операторы
			int operand1 = head->info;
			new_s = head->previous;
			delete head;
			head = new_s;
			int operand2 = head->info;
			switch (c) {
			case '*':
				summ = operand1 * operand2;
				break;
			case '+':
				summ = operand1 + operand2;
				break;
			case '-':
				summ = operand1 - operand2;
				break;
			case '/':
				summ = operand1 / operand2;
				break;
			}
			head->info = summ;
		}
		i--;
	}
	delete head;
	return summ;
}

int normal_record_calculation(char*& expression) {
	Stack<int>* operands = nullptr;//стеки для операндов и неприоритетных операций
	Stack<char>* operators = nullptr;
	Stack<int>* newi;
	Stack<char>* newc;
	unsigned char c = *expression;
	while (!(c == ')' or c == '\0')) {
		if (c == '*' or c == '/' or c == '-' or c == '+') {//занесение операции в стек
			newc = new Stack<char>;
			newc->previous = operators;
			operators = newc;
			operators->info = c;
			expression++;
			c = *expression;
		}
		if (c == '(') {//занесение операнда в стек
			newi = new Stack<int>;
			newi->previous = operands;
			operands = newi;
			operands->info = normal_record_calculation(++expression);//возвращение значения выражения в скобках
			c = *expression;
		}
		else if (c >= 48 and c <= 57) {//занесение операнда в стек
			newi = new Stack<int>;
			newi->previous = operands;
			operands = newi;
			operands->info = char_to_int(expression);
			c = *expression;
		}
		if (operators != nullptr and (operators->info == '*' or operators->info == '/')) {//сначала подсчет приоритетных операций
			if (operators->info == '*')
				operands->previous->info *= operands->info;
			if (operators->info == '/')
				operands->previous->info /= operands->info;
			newi = operands->previous;
			delete operands;
			operands = newi;
			newc = operators->previous;
			delete operators;
			operators = newc;
		}
	}

	if (c == ')')
		expression++;
	while (operands->previous != nullptr) {//подсчет неприоритетных операций
		switch (operators->info) {
		case '+':
			operands->previous->info += operands->info;
			break;
		case '-':
			operands->previous->info -= operands->info;
			break;
		}
		newi = operands->previous;
		delete operands;
		operands = newi;
		newc = operators->previous;
		delete operators;
		operators = newc;
	}
	int summ = operands->info;
	delete operands;
	return summ;
}

//int normal_record_calculation(char*& expression) {//вычисление обычного выражения без стека и приоритета операций
//	char operat = 0;
//	int summ = 0;
//	unsigned char c = *expression;
//	while (!(c == ')' or c == '\0')) {
//		if (c == '(') {
//			switch (operat) {
//			case '*':
//				summ *= normal_record_calculation(++expression);
//				operat = 1;
//				break;
//			case '/':
//				summ /= normal_record_calculation(++expression);
//				operat = 1;
//				break;
//			case '-':
//				summ -= normal_record_calculation(++expression);
//				operat = 1;
//				break;
//			case '+':
//				summ += normal_record_calculation(++expression);
//				operat = 1;
//				break;
//			case 0:
//				summ = normal_record_calculation(++expression);
//				operat = 1;
//				break;
//			case 1:
//				summ *= normal_record_calculation(++expression);
//				break;
//			}
//			c = *expression;
//		}
//
//		if (c == '*' or c == '/' or c == '-' or c == '+')
//			operat = c;
//		if (c >= 48 and c <= 57) {
//			switch (operat) {
//			case '*':
//				summ *= reverse_char_to_int(expression);
//				break;
//			case '/':
//				summ /= reverse_char_to_int(expression);
//				break;
//			case '-':
//				summ -= reverse_char_to_int(expression);
//				break;
//			case '+':
//				summ += reverse_char_to_int(expression);
//				break;
//			case 0:
//				summ = reverse_char_to_int(expression);
//				break;
//			case '1':
//				summ *= reverse_char_to_int(expression);
//				break;
//			}
//			operat = 1;
//			c = *expression;
//		}
//		else if (c != '\0') {
//			expression++;
//			c = *expression;
//		}
//	}
//	if (c == ')')
//		expression++;
//	return summ;
//}

void test_generation(char* filename, int quantity) {
	int q_operands, q_operations, cout_operands;
	ofstream file(filename);
	char* expression = new char[50];
	for (int j = 0; j < quantity; j++) {
		char* c;
		file.write("Вариант ", strlen("Вариант "));
		c = int_to_char(j + 1);
		file.write(c, strlen(c));
		delete[] c;
		file.write("\nРешите выражения:\n1) ", strlen("\nРешите выражения:\n1) "));
		cout_operands = q_operands = 5 + rand() % 4;//рандомное количество операндов в выражении 1, чем больше здесь, тем меньше будет в выражении 2
		q_operations = q_operands - 1;
		for (int i = 0; i < 50; i++)
			expression[i] = '\0';
		char* expression_ptr = expression;
		for (expression_ptr; q_operands or q_operations; 1) {//обратная польская
			if (rand() % 2) {//вероятность добавить операнд - 50%, операцию - 50%, если выполнятся проверки
				if (q_operands) {//если еще остались свободные операнды
					c = int_to_char(rand() % 15 + 1);//выбор значения операнда
					strcat(expression_ptr, c);
					delete[] c;
					while (*expression_ptr != '\0')
						expression_ptr++;
					*expression_ptr = ' ';
					q_operands--;
					expression_ptr++;
				}
			}
			else {
				if (q_operands < q_operations) {		//сели количество операндов меньше количества операций
					*expression_ptr = rand() % 3 + 1;	//соответственно, первые два символа выражения будут операнды (считая операнды больше 9 в скобках за один),
														//а последний будет операцией
					switch (*expression_ptr) {
					case 1:
						*expression_ptr = '+';
						break;
					case 2:
						*expression_ptr = '*';
						break;
					case 3:
						*expression_ptr = '-';
						break;
					}
					q_operations--;
					expression_ptr++;
				}
			}
		}
		file.write(expression, strlen(expression));//запись строки в файл

		file.write("\n2) ", strlen("\n2) "));
		q_operands = 5 + 8 - cout_operands;//количество операндов в соответствии с 1 выражением
		q_operations = q_operands - 1;
		for (int i = 0; i < 50; i++)
			expression[i] = '\0';
		expression_ptr = expression;
		for (expression_ptr; q_operands or q_operations; 1) {//обратная польская
			if (rand() % 2) {
				if (q_operands) {
						for (int i = 49; i > 0; i--)//сдвиг выражения вправо
							expression_ptr[i] = expression_ptr[i - 1];
						*expression_ptr = ' ';
					c = int_to_char(rand() % 15 + 1);
					for (int j = strlen(c); j > 0; j--) {//сдвиг вправо на символьную длину числа и его запись
						for (int i = 49; i > 0; i--)
							expression_ptr[i] = expression_ptr[i - 1];
						*expression = c[j - 1];//соответственно, запись числа с конца
					}
					q_operands--;
				}
			}
			else {
				if (q_operands < q_operations) {
					for (int i = 49; i > 0; i--)
						expression_ptr[i] = expression_ptr[i - 1];
					*expression_ptr = rand() % 3 + 1;//выбор операции
					switch (*expression_ptr) {
					case 1:
						*expression_ptr = '+';
						break;
					case 2:
						*expression_ptr = '*';
						break;
					case 3:
						*expression_ptr = '-';
						break;
					}
					q_operations--;
				}
			}
		}
		file.write(expression, strlen(expression));
		file.write("\n\n", strlen("\n\n"));
	}
	file.close();
	delete[] expression;
}

void stack_output(Stack<int>* head, ofstream* file) {//вывод в файл
	while (head != nullptr) {
		*file << head->info << " ";
		head = head->previous;
	}
}

void stack_output(Stack<int>* head) {//вывод в консоль
	while (head != nullptr) {
		cout << head->info << " ";
		head = head->previous;
	}
}

void stack_delete(Stack<int>* head) {
	Stack<int>* next;
	while (head != nullptr) {
		next = head->previous;
		delete head;
		head = next;
	}
}

void test_solution(char* Testfilename, char* filename) {
	ifstream test(Testfilename);
	ofstream solution(filename);
	char* line = new char[100];
	char* line_ptr;

	while (test.getline(line, 100, '\n')) {
		if (*line == 'В')
			solution << line << endl;//перенос строки с номером варианта
		if (*line == '1' or *line == '2') {
			line_ptr = line;
			line_ptr += 3;//сдвиг указателя к началу выражения
			for (int i = 0; i < strlen(line_ptr); i++) {//перевод записи выражения в приемлемую форму для вычисления
				if (line_ptr[i] == ' ')
					line_ptr[i] = ')';
				if ((unsigned char)line_ptr[i] >= 48 and (unsigned char)line_ptr[i] <= 57) {
					for (int j = 49; j > i; j--)
						line_ptr[j] = line_ptr[j - 1];
					line_ptr[i] = '(';
					do {
						i++;
					} while ((unsigned char)line_ptr[i] >= 48 and (unsigned char)line_ptr[i] <= 57);
					i--;
				}
			}
			if (*line == '2') {//перевод из польской в обратную польскую
				char* new_line_ptr = new char[strlen(line_ptr)];
				for (int i = strlen(line_ptr) - 1; i >= 0; i--) {
					if (line_ptr[i] == '+' or line_ptr[i] == '*' or line_ptr[i] == '-' or line_ptr[i] == '/') {
						*new_line_ptr = line_ptr[i];
						new_line_ptr++;
					}
					if (line_ptr[i] == '(') {
						int j = i;
						while (line_ptr[j] != ')') {
							*new_line_ptr = line_ptr[j];
							new_line_ptr++;
							j++;
						}
						*new_line_ptr = line_ptr[j];
						new_line_ptr++;
					}
				}
				*new_line_ptr = '\0';
				for (int i = strlen(line_ptr) - 1; i >= 0; i--)
					new_line_ptr--;
				line_ptr -= 3;
				line_ptr = new_line_ptr;
			}
			Stack<int>* head = nullptr;
			Stack<int>* new_s = nullptr;
			int summ = 0;
			unsigned char c;
			for (c = *line_ptr; c != '\0'; c = *(++line_ptr)) {//решение выражение и вывод действий в файл
				if (c >= 48 and c <= 57 or c == '(') {//операнды
					new_s = new Stack<int>;
					new_s->previous = head;
					head = new_s;
					if (c != '(')
						head->info = c - 48;
					else {
						head->info = char_to_int(++line_ptr);
					}
					solution << "Заносим в стек число " << head->info << endl;
					solution << "Стек: ";
					stack_output(head, &solution);
					solution << endl;
				}
				else {//операторы
					int operand1 = head->info;
					new_s = head->previous;
					delete head;
					head = new_s;
					int operand2 = head->info;
					solution << "Достаем из стека числа " << operand2 << " и " << operand1 << endl;
					switch (c) {
					case '*':
						summ = operand2 * operand1;
						solution << "Перемножая получаем " << summ << " заносим это число в стек" << endl;
						break;
					case '+':
						summ = operand2 + operand1;
						solution << "Складывая получаем " << summ << " заносим это число в стек" << endl;
						break;
					case '-':
						summ = operand2 - operand1;
						solution << "Вычитая получаем " << summ << " заносим это число в стек" << endl;
						break;
					case '/':
						summ = operand2 / operand1;
						solution << "В результате частного " << summ << " заносим это число в стек" << endl;
						break;
					}
					head->info = summ;
					solution << "Стек: ";
					stack_output(head, &solution);
					solution << endl;
				}
			}
			delete head;
			solution << "Ответ: " << summ << endl << endl;
		}
	}

	test.close();
	solution.close();
	delete[] line;
}

void part4() {
	Stack<int>* head = nullptr;
	Stack<int>* news;
	for (int i = 0; i < 10; i++) {//создание стека длиной 10
		news = new Stack<int>;
		news->previous = head;
		head = news;
		head->info = rand() % 200 - 100;
	}
	cout << "Стэк: ";
	stack_output(head);
	cout << endl;

	Stack<int>* headplus = nullptr;
	Stack<int>* headminus = nullptr;
	while (head != nullptr) {//разделение на два
		if (head->info > 0) {
			news = new Stack<int>;
			news->previous = headplus;
			headplus = news;
			headplus->info = head->info;
		}
		else if (head->info != 0){
			news = new Stack<int>;
			news->previous = headminus;
			headminus = news;
			headminus->info = head->info;
		}
		news = head->previous;
		delete head;
		head = news;
	}
	cout << "Стэк с положительными числами: ";
	stack_output(headplus);
	cout << endl;

	cout << "Стэк с отрицательными числами: ";
	stack_output(headminus);
	cout << endl;
	stack_delete(headplus);
	stack_delete(headminus);
}

int main()
{
	setlocale(LC_ALL, "ru");
	cout << "ПОЛЬЗОВАТЕЛЬСКОЕ СОГЛАШЕНИЕ\n";
	cout << "(1)Вводите только положительные числа\n";
	cout << "(2)Вводите только однозначные числа\n";
	cout << "(3)Если нужно ввести число больше 9, то обозначте его\n переменной, после проверки правильности"
		<< "выражения,\n вы сможете указать значение этой переменной\n";
	cout << "(4)Обочначать переменные можно только строчными английскими буквами\n";
	cout << "(5)Продолжая использовать программу, вы принимаете условия\n пользовательского соглашения\n";
	cout << "---------------------------------------------------------------------\n";
	system("Pause");
	srand(time(0));
	chrono::time_point<chrono::high_resolution_clock> start, end;
	char* expression = new char[100];
	int expression_number, choise;
	double elapsed_seconds;
	ifstream file;
	ofstream file2("file.txt", ios_base::app);
	file2.close();
	bool exit = 0;

	while (!exit) {
		expression_number = 0;
		system("cls");
		cout << "1 - ввод выражения с файла\n";
		cout << "2 - ввод с консоли\n";
		cout << "3 - генерация проверочной работы на прямую и обратную польские нотации с\n ответами\n";
		cout << "4 - Реализуйте стек. Заполните его случайными положительными и\n"
			" отрицательными числами.Преобразуйте стек в два стека.Первый\n"
			" должен содержать только положительные числа, второй – отрицательные.\n";
		cout << "5 - выход из программы\n";
		cin >> choise;
		stream_cleaning();
		if (choise == 1) {
			cout << "Введите выражение:\n";
			system("file.txt");
			file.open("file.txt");
			file.getline(expression, 50, '\n');
			file.close();
		}

		if (choise == 2){
			cout << "Введите выражение:\n";
			cin.getline(expression, 50, '\n');
			stream_cleaning();
		}

		if (choise == 1 or choise == 2) {
			start = chrono::high_resolution_clock::now();
			bool b = check_reverse_polish_notation(expression);
			end = chrono::high_resolution_clock::now();
			if (b) {
				cout << "Выражение записано в виде обратной польской нотации\n";
				elapsed_seconds = chrono::duration_cast<chrono::nanoseconds>(end - start).count();
				cout << "Время проверки обратной польской нотации: " << 0.0000000001 * elapsed_seconds << " секунды\n";
				expression_number = 1;
			}
			if (!b) {
				start = chrono::high_resolution_clock::now();
				b = check_polish_notation(expression);
				end = chrono::high_resolution_clock::now();
				if (b) {
					cout << "Выражение записано в виде прямой польской нотации\n";
					elapsed_seconds = chrono::duration_cast<chrono::nanoseconds>(end - start).count();
					cout << "Время проверки польской нотации: " << 0.0000000001 * elapsed_seconds << " секунды\n";
					expression_number = 2;
				}
			}
			if (!b) {
				start = chrono::high_resolution_clock::now();
				b = check_simple_expression(expression);
				end = chrono::high_resolution_clock::now();
				if (b) {
					cout << "Выражение записано в обычном виде\n";
					elapsed_seconds = chrono::duration_cast<chrono::nanoseconds>(end - start).count();
					cout << "Время проверки обычного выражения: " << 0.0000000001 * elapsed_seconds << " секунды\n";
					expression_number = 3;
				}
			}
			if (!b)
				cout << "Неверное выражение\n";

			if (expression_number) {

				filling_variables(expression);//Ввод переменных
				cout << "Решение выражения " << expression << ": ";

				switch (expression_number) {
				case 1:
					start = chrono::high_resolution_clock::now();
					cout << calculating_reverse_polish_notation(expression) << endl;
					end = chrono::high_resolution_clock::now();
					elapsed_seconds = chrono::duration_cast<chrono::nanoseconds>(end - start).count();
					cout << "Время решения: " << 0.0000000001 * elapsed_seconds << " секунды\n";
					break;
				case 2:
					start = chrono::high_resolution_clock::now();
					cout << calculating_polish_notation(expression) << endl;
					end = chrono::high_resolution_clock::now();
					elapsed_seconds = chrono::duration_cast<chrono::nanoseconds>(end - start).count();
					cout << "Время решения: " << 0.0000000001 * elapsed_seconds << " секунды\n";
					break;
				case 3:
					start = chrono::high_resolution_clock::now();
					char* expression_ptr = expression;
					cout << normal_record_calculation(expression_ptr) << endl;
					end = chrono::high_resolution_clock::now();
					elapsed_seconds = chrono::duration_cast<chrono::nanoseconds>(end - start).count();
					cout << "Время решения: " << 0.0000000001 * elapsed_seconds << " секунды\n";
					break;
				}
			}
		}

		if (choise == 3) {
			test_generation((char*)"control.txt", 10);
			system("control.txt");
            test_solution((char*)"control.txt", (char*)"control_solution.txt");
			system("control_solution.txt");
		}

		if (choise == 4) {
			part4();
		}

		if (choise == 5) {
			exit = 1;
		}

		if (!exit)
			system("Pause");
	}
}
