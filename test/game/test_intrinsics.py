import unittest 

import intrinsicsC

testIntrinsics = {
	"valA": { "aaa" : True,
      "aaZ" : False },
    "valB": { "bbb" :True,
      "bbZ" : False },
    }

class IntrinsicsTest(unittest.TestCase):

	def test_parseOnlyNoResult(self):
	  testEmpty = ""
	  uTestEmpty = testEmpty.decode('utf-8')
	  res = intrinsicsC.DispatchIntrinsics(testIntrinsics, [uTestEmpty])
	  self.assertEqual(len(res), 0)

	def test_found_valA_Enable(self):
	  testInput = "blah aaa blah"
	  uTestInput = testInput.decode('utf-8')
	  res = intrinsicsC.DispatchIntrinsics(testIntrinsics, [uTestInput])
	  self.assertEqual(len(res), 1)
	  self.assertEqual(res.items()[0][0], "valA")
	  self.assertEqual(res.items()[0][1], True)

	def test_found_valA_Disable(self):
	  testInput = "blah aaZ blah"
	  uTestInput = testInput.decode('utf-8')
	  res = intrinsicsC.DispatchIntrinsics(testIntrinsics, [uTestInput])
	  self.assertEqual(len(res), 1)
	  self.assertEqual(res.items()[0][0], "valA")
	  self.assertEqual(res.items()[0][1], False)
	
	def test_found_valA_EnableDisable(self):
	  testInput = "aaa blah aaZ blah"
	  uTestInput = testInput.decode('utf-8')
	  res = intrinsicsC.DispatchIntrinsics(testIntrinsics, [uTestInput])
	  self.assertEqual(len(res), 1)
	  self.assertEqual(res.items()[0][0], "valA")
	  self.assertEqual(res.items()[0][1], False)

	def test_found_valAvalB_Enabled(self):
	  testInput = "aaa blah bbb blah"
	  uTestInput = testInput.decode('utf-8')
	  res = intrinsicsC.DispatchIntrinsics(testIntrinsics, [uTestInput])
	  self.assertEqual(len(res), 2)
	  self.assertEqual(res.items()[0][0], "valB")
	  self.assertEqual(res.items()[0][1], True)
	  self.assertEqual(res.items()[1][0], "valA")
	  self.assertEqual(res.items()[1][1], True)

	def test_found_valBvalA_Enabled(self):
	  testInput = "bbb blah aaa blah"
	  uTestInput = testInput.decode('utf-8')
	  res = intrinsicsC.DispatchIntrinsics(testIntrinsics, [uTestInput])
	  self.assertEqual(len(res), 2)
	  self.assertEqual(res.items()[0][0], "valB")
	  self.assertEqual(res.items()[0][1], True)
	  self.assertEqual(res.items()[1][0], "valA")
	  self.assertEqual(res.items()[1][1], True)

	def test_found_valA_Enabled_valB_Disabled(self):
	  testInput = "aaa blah bbZ blah"
	  uTestInput = testInput.decode('utf-8')
	  res = intrinsicsC.DispatchIntrinsics(testIntrinsics, [uTestInput])
	  self.assertEqual(len(res), 2)
	  self.assertEqual(res.items()[0][0], "valB")
	  self.assertEqual(res.items()[0][1], False)
	  self.assertEqual(res.items()[1][0], "valA")
	  self.assertEqual(res.items()[1][1], True)

if __name__ == "__main__":
  unittest.main()
