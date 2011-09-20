#include <Python.h>

#include <string>
#include <vector>

using namespace std;

namespace
{
    static bool firstTime = true;

    struct TextSearchItem
    {
	TextSearchItem()
	{}

	TextSearchItem(const string& name, const wstring& searchTerm, bool result) 
	    : name_(name), 
	    searchTerm_(searchTerm),
	    result_(result)
	{}

	string name_;
	wstring searchTerm_;
	bool result_; // intrinsic/status enabled or disabled
    };

    typedef vector<TextSearchItem> TextSearchItems;
    static TextSearchItems searchItems;

    string ToStr(PyObject* pyOb)
    {
	// is type-check needed?
	return PyString_AsString(pyOb); 
    }

    wstring ToWStr(PyObject* pyOb)
    {
	// is type-check needed?
	const string str = PyString_AsString(pyOb); 
	return wstring(str.begin(), str.end());
    }

    bool ToBool(PyObject* pyOb)
    {
	// if PyBool_check(pyOb) ... is type-check needed?
	return Py_True == pyOb;
    }

    static void ParseDictionary(PyObject* dictionary)
    {
	firstTime = false; // we don't want to do this again...
	
	PyObject *keys = PyDict_Keys(dictionary);
	for(int i = 0; i < PyList_Size(keys); ++i)
	{
	    PyObject* pyItemName = PyList_GetItem(keys, i);
	    const string itemName = ToStr(pyItemName);

	    PyObject *itemSearchTerms = PyDict_GetItem(dictionary, pyItemName); 
	    if(!PyDict_Check(itemSearchTerms))
	    {
		continue;
	    }

	    PyObject *subKeys = PyDict_Keys(itemSearchTerms);

	    for(int j = 0; j < PyList_Size(subKeys); ++j)
	    {
		PyObject* pySearchTerm = PyList_GetItem(subKeys, j);

		const wstring wsSearchTerm = ToWStr(pySearchTerm);		    
		bool searchTermValue = ToBool(PyDict_GetItem(itemSearchTerms, pySearchTerm));

		searchItems.push_back(TextSearchItem(itemName, wsSearchTerm, searchTermValue));
	    }	    
	}
    }
}

static PyObject* DispatchIntrinsics(PyObject *self, PyObject* args)
{    
    if(PyTuple_Size(args) != 2)
    {
	return PyDict_New();
    }

    if(firstTime)
    {
	PyObject *dictionary = PyTuple_GetItem(args, 0);
	if(PyDict_Check(dictionary))
	{
	    ParseDictionary(dictionary);
	}
	else
	{
	    return NULL;
	}
    }

    typedef pair<string, bool> StdResult;
    typedef vector< StdResult > StdResults;
    StdResults stdResults;

    PyObject *messageData = PyTuple_GetItem(args, 1);
    if(PyList_Check(messageData))
    {
	int listSize = PyList_Size(messageData);

	for(int i=0 ; i < listSize ; ++i) 
	{
	    PyObject* line = PyList_GetItem(messageData, i);
	    if(!PyUnicode_Check(line))
	    {
		continue;
	    }

	    const wstring wsLine( reinterpret_cast< wchar_t* >( PyUnicode_AS_UNICODE(line) ) );
	
	    if(wsLine == L"")
	    {
		continue;
	    }

	    for(TextSearchItems::const_iterator it = searchItems.begin();
		    it != searchItems.end();
		    ++it)
	    {    
		if(string::npos != wsLine.find(it->searchTerm_))
		{	
		    const string name(it->name_);
		    const wstring wsName(name.begin(), name.end());

		    if(it->result_)
		    {
			stdResults.push_back( StdResult(it->name_, true) );
		    }			 
		    else
		    { 
			stdResults.push_back( StdResult(it->name_, false) );
		    }
		}			    
	    }
	}
    }

    PyObject* pyResults = PyDict_New();
    for(StdResults::const_iterator it = stdResults.begin(); it != stdResults.end(); ++it)
    {	   
	const string name(it->first);
        PyDict_SetItem(pyResults, PyString_FromString(name.c_str()), it->second ? Py_True : Py_False);
    }
    return pyResults;
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

