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

