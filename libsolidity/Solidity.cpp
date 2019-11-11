/*
	This file is part of solidity.

	solidity is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	solidity is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with solidity.  If not, see <http://www.gnu.org/licenses/>.
*/
/**
 * @author Christian <c@ethdev.com>
 * @date 2014
 * Solidity commandline compiler.
 */

#include <clocale>
#include <iostream>
#include <boost/exception/all.hpp>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>

#include <libsolidity/interface/Version.h>
#include <libsolidity/parsing/Scanner.h>
#include <libsolidity/parsing/Parser.h>
#include <libsolidity/ast/ASTPrinter.h>
#include <libsolidity/ast/ASTJsonConverter.h>
#include <libsolidity/analysis/NameAndTypeResolver.h>
#include <libsolidity/interface/Exceptions.h>
#include <libsolidity/interface/CompilerStack.h>
#include <libsolidity/interface/StandardCompiler.h>
#include <libsolidity/interface/SourceReferenceFormatter.h>
#include <libsolidity/interface/GasEstimator.h>
#include <libsolidity/interface/AssemblyStack.h>

#include <libevmasm/Instruction.h>
#include <libevmasm/GasMeter.h>

#include <libdevcore/Common.h>
#include <libdevcore/CommonData.h>
#include <libdevcore/CommonIO.h>
#include <libdevcore/JSON.h>


#include "Solidity.h"


using namespace std;
using namespace dev;
using namespace solidity;

#define NODE_NAME ":Node"

std::string readCodeFile(std::string const& _file)
{
	std::string ret;
	size_t const c_elementSize = sizeof(typename std::string::value_type);
	std::ifstream is(_file, std::ifstream::binary);
	if (!is)
		return ret;

	// get length of file:
	is.seekg(0, is.end);
	streamoff length = is.tellg();
	if (length == 0)
		return ret; // do not read empty file (MSVC does not like it)
	is.seekg(0, is.beg);

	ret.resize((length + c_elementSize - 1) / c_elementSize);
	is.read(const_cast<char*>(reinterpret_cast<char const*>(ret.data())), length);
	return ret;
}

std::ostream& serr(bool _used = true)
{
	(void)_used;
	return cerr;
}

std::string compileNodeAbi()
{

	ReadCallback::Callback fileReader = [](string const& _path)
	{
		try
		{
			auto path = boost::filesystem::path(_path);
			auto canonicalPath = boost::filesystem::canonical(path);
			
			auto contents = readCodeFile(canonicalPath.string());
			return ReadCallback::Result{true, contents};
		}
		catch (Exception const& _exception)
		{
			return ReadCallback::Result{false, "Exception in read callback: " + boost::diagnostic_information(_exception)};
		}
		catch (...)
		{
			return ReadCallback::Result{false, "Unknown exception in read callback."};
		}
	};

	CompilerStack compiler(fileReader);
	auto scannerFromSourceName = [&](string const& _sourceName) -> solidity::Scanner const& { return compiler.scanner(_sourceName); };
	SourceReferenceFormatter formatter(cerr, scannerFromSourceName);

	try{

		std::string name = NODE_NAME;
		compiler.addSource(name, c_node_code);

		bool successful = compiler.compile();
		//cout << "successful=" << successful << endl;
		if(successful){
			vector<string> contracts = compiler.contractNames();

			for (string const& contractName: contracts){
				if(contractName.substr(name.length()) == name){
					Json::Value evmData(Json::objectValue);

					evmData["abi"] = compiler.contractABI(contractName);
					evmData["address"] = nodeAddress();
					evmData["gasEstimates"] = compiler.gasEstimates(contractName);
					//cout << "contractName=" << contractName << ",runtimeObject=" << runtimeObject << endl;

					Json::FastWriter fastWriter;
					std::string output = fastWriter.write(evmData);

					return output;
				}
			}
			
		}

	}
	catch (CompilerError const& _exception)
	{
		formatter.printExceptionInformation(_exception, "Compiler error");
	}
	catch (InternalCompilerError const& _exception)
	{
		cerr << "Internal compiler error during compilation:" << endl
			 << boost::diagnostic_information(_exception);
	}
	catch (UnimplementedFeatureError const& _exception)
	{
		cerr << "Unimplemented feature:" << endl
			 << boost::diagnostic_information(_exception);
	}
	catch (Error const& _error)
	{
		if (_error.type() == Error::Type::DocstringParsingError)
			cerr << "Documentation parsing error: " << *boost::get_error_info<errinfo_comment>(_error) << endl;
		else
			formatter.printExceptionInformation(_error, _error.typeName());
	}
	catch (Exception const& _exception)
	{
		cerr << "Exception during compilation: " << boost::diagnostic_information(_exception) << endl;
	}
	catch (...)
	{
		cerr << "Unknown exception during compilation." << endl;
	}
	
	return "";
}

std::string compileNode()
{
	return compileCode(NODE_NAME, c_node_code);
}

std::string nodeAddress()
{
	return "0x0000000000000000000000000000000000000110";
}

std::string compileCode(std::string _name, std::string _code)
{
	ReadCallback::Callback fileReader = [](string const& _path)
	{
		try
		{
			auto path = boost::filesystem::path(_path);
			auto canonicalPath = boost::filesystem::canonical(path);
			
			auto contents = readCodeFile(canonicalPath.string());
			return ReadCallback::Result{true, contents};
		}
		catch (Exception const& _exception)
		{
			return ReadCallback::Result{false, "Exception in read callback: " + boost::diagnostic_information(_exception)};
		}
		catch (...)
		{
			return ReadCallback::Result{false, "Unknown exception in read callback."};
		}
	};

	CompilerStack compiler(fileReader);
	auto scannerFromSourceName = [&](string const& _sourceName) -> solidity::Scanner const& { return compiler.scanner(_sourceName); };
	SourceReferenceFormatter formatter(cerr, scannerFromSourceName);

	try{
		std::map<std::string, std::string> sourceCodes;
		compiler.addSource(_name, _code);
		//compiler.setOptimiserSettings(true, 200);

		bool successful = compiler.compile();
		cout << (const char*)__FUNCTION__ << ":" << __LINE__ << "::" << "successful=" << successful << endl;

		for (auto const& error: compiler.errors()){
			formatter.printExceptionInformation(
				*error,
				(error->type() == Error::Type::Warning) ? "Warning" : "Error"
			);
		}

		(void)successful;
		//if(successful)
		{
			vector<string> contracts = compiler.contractNames();

			for (string const& contractName: contracts){
				cout << (const char*)__FUNCTION__ << ":" << __LINE__ << "::" << "contractName=" << contractName << ",contractName.substr(_name.length()):" << contractName.substr(_name.length()) << ",_name:" << _name << endl;
				if(contractName.substr(_name.length()) == _name){
					auto runtimeObject = compiler.runtimeObject(contractName).toHex();
					cout << (const char*)__FUNCTION__ << ":" << __LINE__ << "::" << "contractName=" << contractName << endl;
					return runtimeObject;
				}
			}
			
		}

	}
	catch (CompilerError const& _exception)
	{
		formatter.printExceptionInformation(_exception, "Compiler error");
	}
	catch (InternalCompilerError const& _exception)
	{
		cerr << "Internal compiler error during compilation:" << endl
			 << boost::diagnostic_information(_exception);
	}
	catch (UnimplementedFeatureError const& _exception)
	{
		cerr << "Unimplemented feature:" << endl
			 << boost::diagnostic_information(_exception);
	}
	catch (Error const& _error)
	{
		if (_error.type() == Error::Type::DocstringParsingError)
			cerr << "Documentation parsing error: " << *boost::get_error_info<errinfo_comment>(_error) << endl;
		else
			formatter.printExceptionInformation(_error, _error.typeName());
	}
	catch (Exception const& _exception)
	{
		cerr << "Exception during compilation: " << boost::diagnostic_information(_exception) << endl;
	}
	catch (...)
	{
		cerr << "Unknown exception during compilation." << endl;
	}

	return "";
}

std::string compileFile(std::string _name, std::string _contract_name)
{
	string code = readCodeFile(_name);
	
	if(code.empty())
		return std::string();

	return compileCode(_contract_name, code);
}

