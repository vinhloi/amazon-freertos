// Generate R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_XX() macros for ICCRX
// cscript -nologo gen_iccrx_R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_XX.js > gen_iccrx_R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_XX.h

var stdout = WScript.StdOut;
var NL = "\r\n";
var str = "";

for (var argnum = 2; argnum <= 31; argnum++)
{
	str += "#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_" + argnum + "(";
	for (var i = 1; i <= argnum; i++)
	{
		str += "bf" + i;
		if (i < argnum)
		{
			str += ", ";
		}
		else
		{
			str += ")\\" + NL;
		}
	}
	str += "struct {\\" + NL;
	str += "R_PRAGMA(bitfield=reversed_disjoint_types)\\" +NL;
	str += "    struct {\\" + NL;
	for (var i = 1; i <= argnum; i++)
	{
		str += "        bf" + i + ";\\" + NL;
	}
	str += "    };\\" + NL;
	str += "R_PRAGMA(bitfield=default)\\" + NL;
	str += "}" + NL;
	str += NL;
}

stdout.Write(str);
