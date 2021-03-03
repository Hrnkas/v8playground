// Test_V8.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>



#include <v8pp/context.hpp>
#include <v8pp/module.hpp>
#include <v8pp/class.hpp>
#include <v8pp/property.hpp>
#include <libplatform/libplatform.h>
#include <v8.h>

using namespace v8pp;

class Insertion
{
public:
	Insertion(int integer, std::string string) : integer_(integer), string_(string) {};

	//getters/setters
	int getInt() { return integer_; }
	void setInt(int integer) { integer_ = integer; }

	std::string getStr() { return string_; }
	void setStr(int string) { string_ = string; }

	//methods
	int add(int number) { return integer_ + number; }

protected:
	int integer_;
	std::string string_;
};

int main()
{
	v8::V8::InitializeExternalStartupData(__argv[0]);

	auto platform = v8::platform::NewDefaultPlatform();
	v8::V8::InitializePlatform(platform.get());
	v8::V8::Initialize();

	//we need scope here, so that context destructor is called before v8::V8::Dispose() and v8::V8::ShutdownPlatform()
	{
		context context;
		v8::Isolate* isolate = context.isolate();
		v8::HandleScope handleScope(isolate);

		module testlib(isolate);

		// bind class
		v8pp::class_<Insertion> Insertion_class(isolate);
		Insertion_class
			// specify constructor signature
			.ctor<int, std::string>()
			.set("add", &Insertion::add)
			// bind property
			.set("Integer", property(&Insertion::getInt, &Insertion::setInt))
			.set("String", property(&Insertion::getStr, &Insertion::setStr));

		// set class into the module template
		testlib.set("Insertion", Insertion_class);

		// set bindings in global object as `mylib`
		isolate->GetCurrentContext()->Global()->Set(
			v8::String::NewFromUtf8(isolate, "testlib"), testlib.new_instance());

		auto result = context.run_script(
			"function fib(n) {"
			"if (n == 0) {"
			"return 0;"
			"}"
			"else if (n == 1) {"
			"return 1;"
			"}"

			"return fib(n - 1) + fib(n - 2);"
			"}"

			"var ins = new testlib.Insertion(3, 'test');"
			"ins.Integer = ins.add(5);"
			"fib(ins.Integer);"
		);

		v8::String::Utf8Value resultString(context.isolate(), result);

		std::cout << "The result of the script was: " << *resultString << "\n";
	}
    //std::cout << "Hello World!\n";

	v8::V8::Dispose();
	v8::V8::ShutdownPlatform();
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
