var shell = require("shelljs");
var resolve = require("path").resolve;

function replaceRange(content, begin, end, replace) {
    var lines = content.split("\n");
    
    lines = lines.reduce((acc, line) => {
        switch (acc.status) {
            case "Null":
                if (line.includes(begin)) {
                    acc.status = "Removing";  
                    acc.result.push(replace);
                } else {
                    acc.result.push(line);
                }
                break;
            case "Removing":
                if (line.includes(end)) {
                    acc.status = "Finished"   
                }                
                break;                
            case "Finished":
                acc.result.push(line);
                break;                
        };
        return acc;
    }, {result:[], status: "Null"}).result;
    
    return lines.join("\n");
}

var source = resolve(__dirname, "../src/cpp/underline.h");
var content = shell.cat(source).toString();

var result = replaceRange(content, "BEGIN_GENERATED_CODE", "END_GENERATED_CODE", "/// __GENERATOR__")

shell.ShellString(result).to(source);
