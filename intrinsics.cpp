#include <fstream>
#include <string>
#include <map>

#include <Python.h>

using namespace std;

namespace
{
    static bool firstTime = true;

    struct Effect
    {
        string intrinsicName_;
        bool result_; // enabled or disabled
    };

    static map<string, Effect> effects;

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

    PyObject *messageData = PyTuple_GetItem(args, 1);
    if(PyList_Check(messageData))
    {
        fs << L"message data is a list (now expected...)\n";
        int listSize = PyList_Size(messageData);
        fs << L"Size of list: " << listSize << endl;
        for(int i=0;i<listSize;++i) 
        {
            PyObject* line = PyList_GetItem(messageData, i);

            if(PyUnicode_Check(line))
            {
                const wstring wsLine(reinterpret_cast<wchar_t*>(PyUnicode_AS_UNICODE(line)));
                fs << L"line (isUnicode) " << i << L" = " << wsLine << endl;
            }
        }
    }

    bool added = true;
    PyObject* results = PyDict_New();
    PyObject* trueFalse = added ? Py_True : Py_False;
    int res = PyDict_SetItem(results, PyString_FromString("Keyvalue"), trueFalse);
    if(res != 0)
    {
        fs << L"pydict_setitem failed!\n";
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

