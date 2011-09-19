import unittest 

import intrinsicsC

testIntrinsics = {
	"valA": { "aaa" : True,
      "aaZ" : False },
    "valB": { "bbb" :True,
      "bbZ" : False },
    }

class IntrinsicsTest(unittest.TestCase):
#  def test_nullTestFail(self):
#	self.assertEqual(1, 0)
#
#  def test_nullTestPass(self):
#	self.assertEqual(1,1)

	def test_parse(self):
	  testEmpty = ""
	  uTestEmpty = testEmpty.decode('utf-8')
	  res = intrinsicsC.DispatchIntrinsics(testIntrinsics, [uTestEmpty])

	def test_found_valA(self):
	  testInput = "blah aaa blah"
	  uTestInput = testInput.decode('utf-8')
	  res = intrinsicsC.DispatchIntrinsics(testIntrinsics, [uTestInput])
	  self.assertEqual(len(res), 1)
	  self.assertEqual(res.items()[0][0], "valA")
	  self.assertEqual(res.items()[0][1], True)

if __name__ == "__main__":
  unittest.main()
