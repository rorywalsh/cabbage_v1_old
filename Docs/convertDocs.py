from os import listdir
import os.path
import os
from os.path import isfile, join, basename
docfiles = [ f for f in listdir(".") if isfile(join(".",f)) ]

for filename in docfiles:
	if ".md" in filename:
		inputFile = open(filename)
		outputFile = open("web/"+inputFile.name, 'w')
		lineText = ""

		# add YAML
		fileNameOnly = os.path.basename(inputFile.name)
		outputFile.write("---\n")
		outputFile.write("layout: docs\n")
		outputFile.write("title: "+os.path.splitext(fileNameOnly)[0]+"\n")
		outputFile.write("permalink: /docs/"+os.path.splitext(fileNameOnly)[0]+"/\n")
		outputFile.write("---\n\n")

		# now parse files and update to Gordon's online format
		for line in inputFile:
			if "**" in line:
				lineText = line.replace("**", "`")
			elif "(images/" in line:
				lineText = "![]({{ site.url }}/images/docs/"+line[line.index("(images/")+8:]
			elif "```html" in line:
				lineText = line.replace("```html", "```csharp")
			elif "[Identifier channels](./identchannels.md)" in line:
				lineText = line.replace("[Identifier channels](./identchannels.md)", "[Identifier channels](../identchannels/index.html)")
			elif "[Widget arrays](./widget_arrays.md)" in line:
				lineText = line.replace("[Widget arrays](./widget_arrays.md)", "[Widget arrays](../widget_arrays/index.html)")
			else:
				lineText = line
			outputFile.write(lineText)

		inputFile.close()
		outputFile.close()

# inputFile = open("opcodes.txt")
# outputFile = open("db/csound.json", 'w')
# outputFile.write("{\n")

# [Identifier channels](../identchannels/index.html)  
# [Widget arrays](./widget_arrays.md)
# for line in inputFile:
# 	tokens = line.split(";")
# 	#entry name
# 	entry = "\t"+tokens[0] + ": {\n"
# 	#descr
# 	entry = entry+"\t\t\"descr\": \""+tokens[2][1:-1].translate(None, "\"")+"\",\n"
# 	#name
# 	entry = entry+"\t\t\"name\": "+tokens[0]+",\n"
# 	#params
# 	entry = entry+"\t\t\"params\": [],\n"
# 	#path
# 	entry = entry+"\t\t\"path\": \"csound/"+tokens[0][1:-1]+".html\",\n"
# 	#syntax
# 	entry = entry+"\t\t\"syntax\": \""+tokens[3][1:-1].translate(None, "\"")+"\",\n"
# 	#type
# 	entry = entry+"\t\t\"type\": "+tokens[1]+"\n"
# 	entry = entry+"\t},\n"
# 	outputFile.write(entry)
	

# outputFile.write("}")
# outputFile.close()
# #now sort out trailing ,
# inputFile = open("db/csound.json")
# dbFile = inputFile.readlines()
# numberOfLines = len(dbFile)

# dbFile[numberOfLines-2] = "\t}\n"

# outputFile = open("db/csound.json", 'w')

# for lines in dbFile:
#   outputFile.write("%s" % lines)

# outputFile.close()
# inputFile.close()

