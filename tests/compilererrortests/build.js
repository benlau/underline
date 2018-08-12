var shell = require("shelljs");
var resolve = require("path").resolve;
var Mustache = require("mustache");

var errors =[
    "_::countBy(\"\", [](){});",
    "_::map(123, [](){});",
    "_::map(std::vector<int>{1,2,3}, [](int, std::string) -> bool {});",
    "_::map(std::vector<int>{1,2,3}, [](int, std::string) {});"
]

var buildDir = resolve(__dirname, "../../build-compilererrortests")
var files = ["template.pro", "template.cpp"]

shell.rm("-rf", buildDir);
shell.mkdir(buildDir);
shell.config.silent = true;

var make = process.platform === "win32" ? "nmake" : "make"

errors.forEach((errorCode) => {
    var view = {
        code: errorCode
    };
    
    files.forEach((file) => {
        var input = shell.cat(resolve(__dirname, file));
        var output = Mustache.render(input.toString(), view);
        shell.ShellString(output).to(resolve(buildDir, file));
    });
    
    shell.exec(["qmake", resolve(buildDir, "template.pro")].join(" "))
    var res = shell.exec(make, {silent: true});

    var output;
    if (process.platform !== "win32") {
        output = res.stderr.toString().split("\n").filter((item) => {return item.indexOf("error:") >= 0});
    } else {
        output = res.stdout.toString().split("\n").filter((item) => {return item.indexOf(" error ") >= 0});        
    }

    console.log(errorCode);
    console.log(output.join("\n"));
    console.log("\n");
})