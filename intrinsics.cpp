#include <fstream>
#include <string>
#include <map>
#include <vector>

#include <Python.h>

using namespace std;

namespace
{
    static bool firstTime = true;

    struct Effect
    {
	Effect()
	{}

	Effect(const string& name, bool result) : intrinsicName_(name), result_(result)
	{}

	string intrinsicName_;
	bool result_; // enabled or disabled
    };

    typedef map<wstring, Effect> EffectsMap;
    static EffectsMap effects;

    static void ParseDictionary(PyObject* intrinsicsDictionary, wofstream& fs)
    {
	PyObject *keys = PyDict_Keys(intrinsicsDictionary);
	for(int i = 0; i < PyList_Size(keys); ++i)
	{
	    PyObject *thisKey = PyList_GetItem(keys, i);
	    PyObject *thisValue = PyDict_GetItem(intrinsicsDictionary, thisKey); 
	    if(PyDict_Check(thisValue))
	    {
		const string keyString = PyString_AsString(thisKey); 
		const wstring wsKeyString(keyString.begin(), keyString.end());

		PyObject *subKeys = PyDict_Keys(thisValue);

		for(int j = 0; j < PyList_Size(subKeys); ++j)
		{
		    PyObject *thisSubKey = PyList_GetItem(subKeys, j);

		    const string subKeyString = PyString_AsString(thisSubKey); 
		    const wstring wsSubKeyString(subKeyString.begin(), subKeyString.end());


		    PyObject *thisSubValue = PyDict_GetItem(thisValue, thisSubKey);
		    if(thisSubValue)
		    {
			if(PyBool_Check(thisSubValue))
			{

			    fs << L"Search Text: " << wsSubKeyString;
			    fs << L" value: " << (thisSubValue == Py_True ? L"enable" : L"disable");
			    fs << L" " << wsKeyString << endl;

			    effects[wsSubKeyString] = Effect(keyString, thisSubValue == Py_True);
			}
		    }
		    else
		    {
			fs << L"value for " << wsSubKeyString << L" wasn't found" << endl;
		    }
		}
	    }
	}
    }
}

static PyObject* DispatchIntrinsics(PyObject *self, PyObject* args)
{    
    wofstream fs("out.txt", ios::app);
    fs << L"In DispatchIntrinsics\n";

    if(PyTuple_Size(args) != 2)
    {
	fs << L"Two args expected (found " << PyTuple_Size(args) << L")!\n";
	return PyDict_New();
    }

    if(firstTime)
    {
	fs << L"First time! Collating data!" << endl;

	PyObject *intrinsicsDictionary = PyTuple_GetItem(args, 0);
	if(PyDict_Check(intrinsicsDictionary))
	{
	    ParseDictionary(intrinsicsDictionary, fs);
	}
	else
	{
	    fs << L" Intrinsics dictionary isn't a dictionary; we're buggered\n";
	    return NULL;
	}
    }

    vector<string> added;
    vector<string> removed;

    PyObject *messageData = PyTuple_GetItem(args, 1);
    if(PyList_Check(messageData))
    {
	int listSize = PyList_Size(messageData);

	fs << L"Number of rows: " << listSize << endl;

	for(int i=0;i<listSize;++i) 
	{
	    PyObject* line = PyList_GetItem(messageData, i);

	    if(PyUnicode_Check(line))
	    {
		const wstring wsLine(reinterpret_cast<wchar_t*>(PyUnicode_AS_UNICODE(line)));
		fs << L"line (unicode) " << i << L" = " << wsLine << endl;

		for(EffectsMap::const_iterator it = effects.begin();
			it != effects.end();
			++it)
		{    
		    const wstring& searchterm = (*it).first;

		    fs << L"testing for " << searchterm << endl;
		    if(string::npos != wsLine.find(searchterm))
		    {			
			if(it->second.result_)
			{
			    fs << L"found an Enable term" 
				<< L"for " <<  wstring(it->second.intrinsicName_.begin(), it->second.intrinsicName_.end())
				<< endl;					 
			    added.push_back(it->second.intrinsicName_);
			}			 
			else
			{ 
			    fs << L"found a Disable term" 
				<< L"for " <<  wstring(it->second.intrinsicName_.begin(), it->second.intrinsicName_.end())
				<< endl;		
			    removed.push_back(it->second.intrinsicName_);
			}
		    }			    
		}
	    }
	}
    }

    PyObject* results = PyDict_New();
    for(vector<string>::const_iterator it = added.begin(); it != added.end(); ++it)
    {	    
	int res = PyDict_SetItem(results, PyString_FromString(it->c_str()), Py_True);
	if(res != 0)
	{
	    fs << L"pydict_setitem failed!\n";
	}
    }
    for(vector<string>::const_iterator it = removed.begin(); it != removed.end(); ++it)
    {	    
	int res = PyDict_SetItem(results, PyString_FromString(it->c_str()), Py_False);
	if(res != 0)
	{
	    fs << L"pydict_setitem failed!\n";
	}
    }
    return results;
}       


static PyMethodDef IntrinsicsMethods[] = 
{
    {"DispatchIntrinsics", DispatchIntrinsics, METH_VARARGS, "Figure out Intrinsics changed..."},
    {NULL, NULL, 0, NULL}
};

PyMODINIT_FUNC initintrinsicsC()
{
    (void) Py_InitModule("intrinsicsC", IntrinsicsMethods);
}

