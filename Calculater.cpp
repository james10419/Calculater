//2021203094유영준



#include <iostream>
#include <stack>
#include <string>
#include <sstream>
#include <stdexcept>
#include <cctype>
#include <cmath>
#include <vector>
#include <iomanip>

// 계산 과정을 저장할 구조체
struct CalculationStep {
    std::string description;
    std::string stackState;
};

class ExpressionEvaluator {
private:
    std::vector<CalculationStep> steps;  // 계산 과정 저장
    bool debugMode;  // 디버그 모드 플래그

    // 스택 상태를 문자열로 변환
    std::string getStackState(const std::stack<int>& s) {
        std::string result = "스택: [";
        std::stack<int> temp = s;
        std::vector<int> values;
        while (!temp.empty()) {
            values.push_back(temp.top());
            temp.pop();
        }
        for (auto it = values.rbegin(); it != values.rend(); ++it) {
            result += std::to_string(*it);
            if (it + 1 != values.rend()) result += ", ";
        }
        return result + "]";
    }

    // 연산자 우선순위 확인 (음수 부호 처리 추가)
    int getPrecedence(char op) const {
        switch (op) {
        case '^': return 4;
        case 'u': return 3;  // 단항 연산자
        case '*': case '/': case '%': return 2;
        case '+': case '-': return 1;
        default: return 0;
        }
    }

    // 개선된 연산자 적용 함수
    int applyOperator(int a, int b, char op) const {
        switch (op) {
        case '+': return a + b;
        case '-': return a - b;
        case '*': return a * b;
        case '^': return static_cast<int>(pow(a, b));
        case '/':
            if (b == 0) throw std::runtime_error("0으로 나눌 수 없습니다");
            return a / b;
        case '%':
            if (b == 0) throw std::runtime_error("0으로 나눈 나머지를 구할 수 없습니다");
            return a % b;
        case 'u': return -a;  // 단항 마이너스
        default: throw std::runtime_error("알 수 없는 연산자입니다");
        }
    }

    // 연산자인지 확인하는 함수 추가
    bool isOperator(char c) const {
        return c == '+' || c == '-' || c == '*' || c == '/' || c == '%' || c == '^' || c == 'u';
    }

    // 중위표기식을 후위표기식으로 변환 (음수 처리 추가)
    std::string toPostfix(const std::string& infix) {
        if (debugMode) {
            std::cout << "입력된 중위표기식: " << infix << std::endl;
        }

        std::stack<char> operators;
        std::string postfix;
        bool expectOperand = true;  // 피연산자 예상 플래그

        for (size_t i = 0; i < infix.length(); ++i) {
            char c = infix[i];

            if (std::isspace(c)) continue;

            if (std::isdigit(c) || (c == '-' && expectOperand && i + 1 < infix.length() && std::isdigit(infix[i + 1]))) {
                bool isNegative = false;
                if (c == '-') {
                    isNegative = true;
                    i++;
                    c = infix[i];
                }
                std::string number = isNegative ? "-" : "";
                while (i < infix.length() && (std::isdigit(infix[i]) || infix[i] == '.')) {
                    number += infix[i++];
                }
                postfix += number + " ";
                --i;
                expectOperand = false;
            }
            else if (c == '(') {
                operators.push(c);
                expectOperand = true;
            }
            else if (c == ')') {
                while (!operators.empty() && operators.top() != '(') {
                    postfix += operators.top();
                    postfix += " ";
                    operators.pop();
                }
                if (!operators.empty()) operators.pop();
                expectOperand = false;
            }
            else if (isOperator(c)) {
                if (c == '-' && expectOperand) {
                    operators.push('u');  // 단항 마이너스로 처리
                }
                else {
                    while (!operators.empty() && getPrecedence(operators.top()) >= getPrecedence(c)) {
                        postfix += operators.top();
                        postfix += " ";
                        operators.pop();
                    }
                    operators.push(c);
                }
                expectOperand = true;
            }
        }

        while (!operators.empty()) {
            postfix += operators.top();
            postfix += " ";
            operators.pop();
        }

        if (debugMode) {
            std::cout << "변환된 후위표기식: " << postfix << std::endl;
        }

        return postfix;
    }

    // 후위표기식 계산 (계산 과정 표시 추가)
    int evaluatePostfix(const std::string& postfix) {
        std::stack<int> operands;
        std::istringstream iss(postfix);
        std::string token;
        int stepCount = 1;

        while (iss >> token) {
            if (std::isdigit(token[0]) || (token[0] == '-' && token.length() > 1)) {
                int value = std::stoi(token);
                operands.push(value);
                if (debugMode) {
                    steps.push_back({
                        std::to_string(stepCount++) + ") Push: " + token,
                        getStackState(operands)
                        });
                }
            }
            else if (token.length() == 1 && isOperator(token[0])) {
                if (token[0] == 'u') {
                    if (operands.empty())
                        throw std::runtime_error("잘못된 수식입니다");
                    int a = operands.top(); operands.pop();
                    int result = applyOperator(a, 0, 'u');
                    operands.push(result);
                }
                else {
                    if (operands.size() < 2)
                        throw std::runtime_error("잘못된 수식입니다");

                    int b = operands.top(); operands.pop();
                    int a = operands.top(); operands.pop();
                    int result = applyOperator(a, b, token[0]);
                    operands.push(result);
                }

                if (debugMode) {
                    std::string op_str = std::string(1, token[0]);
                    steps.push_back({
                        std::to_string(stepCount++) + ") " + (token[0] == 'u' ? "Unary -" : std::to_string(operands.top())),
                        getStackState(operands)
                        });
                }
            }
        }

        if (operands.size() != 1)
            throw std::runtime_error("잘못된 수식입니다");

        return operands.top();
    }

public:
    ExpressionEvaluator(bool debug = false) : debugMode(debug) {}

    void setDebugMode(bool debug) {
        debugMode = debug;
    }

    int evaluate(const std::string& expression) {
        steps.clear();
        try {
            std::string postfix = toPostfix(expression);
            int result = evaluatePostfix(postfix);

            if (debugMode) {
                std::cout << "\n계산 과정:" << std::endl;
                for (const auto& step : steps) {
                    std::cout << std::setw(4) << "" << step.description << std::endl;
                    std::cout << std::setw(8) << "" << step.stackState << std::endl;
                }
                std::cout << "\n최종 결과: " << result << std::endl;
            }

            return result;
        }
        catch (const std::exception& e) {
            std::cout << "에러! (" << e.what() << ")" << std::endl;
            return 0;
        }
    }
};

int main() {
    ExpressionEvaluator evaluator(true);  // 디버그 모드 활성화
    std::string line;

    std::cout << "수식을 입력하세요 (종료하려면 EOI 입력):\n";
    std::cout << "지원되는 연산자: +, -, *, /, %, ^" << std::endl;
    std::cout << "괄호: (, ), [, ], {, }" << std::endl;

    while (std::getline(std::cin, line)) {
        if (line == "EOI") break;
        evaluator.evaluate(line);
        std::cout << "\n다음 수식을 입력하세요:\n";
    }

    return 0;
}
