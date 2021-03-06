var sprintf = require("sprintf-js").sprintf;
var _ = require("lodash");
var shell = require("shelljs");
var resolve = require("path").resolve;
var Mustache = require("mustache");

var content = [];

function argType(value) {
    return value === 0 ? "Functor" : sprintf("Arg%d", value);
};

function templateDeclLine(count) {
    var message = "template <%s>";

    var args = _.range(count).map( item => "typename " + argType(item));
    
    return sprintf(message, args.join(", "));
}

function argsDeclLine(count, actualCount) { // Functor functor, 
    return _.range(count).map( item => {
        if (actualCount !== undefined && item >= actualCount) {
            return argType(item);
        }
        return argType(item) + " " + argType(item).toLowerCase()
    }).join(", ")
}

function templateArgLine(count) { // Functor, Arg1, ...
    return _.range(count).map( item => argType(item)).join(", ")    
}

function argsPassingLine(count) {
    return _.range(count).filter( value => value !== 0 ).map( item=> argType(item).toLowerCase() ).join(", ")
}

function create_function_set(count) { // no. of template argument include Functor

    
    _invoke = (count, actualCount) => {
        // actual count - the actual no. of argument of that function could take

        content.push(templateDeclLine(count));
        content.push(sprintf("inline auto invoke(%s) -> ",argsDeclLine(count, actualCount)));
        content.push(sprintf("typename std::enable_if<is_args_compatible<%s>::value,", templateArgLine(actualCount)));
        content.push(sprintf("typename ret_func<%s>::type>::type {", templateArgLine(actualCount)));        
        content.push(sprintf("    return functor(%s);", argsPassingLine(actualCount)));
        content.push("}\n");
    }   
    
    _is_invokable = (count) => {
        var is_args_compatible = (count) => {
            return sprintf("is_args_compatible<%s>::value", templateArgLine(count));
        };
        
        var condition = _.range(count).map(value => is_args_compatible(value+1));
        
        if (count > 1) {
            condition.push("is_readable<Arg1, Functor>::value"); 
        }
        
        content.push(templateDeclLine(count));
        content.push(sprintf("struct is_invokable%d {", count - 1));
        content.push("    enum {");
        content.push(sprintf("       value = %s" , condition.join(" || ")));
        content.push("    };");

        content.push("};\n");
    };
    
    _invoke_null = (count) => {
        var is_args_not_compatible = (count) => {            
            return sprintf("!is_args_compatible<%s>::value", templateArgLine(count));
        };
        
        var args = _.range(count).map(value => is_args_not_compatible(value+1));
        if (count > 1) {
           args.push("!is_readable<Arg1, Functor>::value");
        }

        content.push(templateDeclLine(count));
        content.push(sprintf("inline auto invoke(%s) -> ",argsDeclLine(count, 0)));
        
        content.push(sprintf("typename std::enable_if<%s,", args.join(" && ")));
        content.push(sprintf("Undefined>::type {"));   
        content.push(sprintf("    return Undefined();"));
        content.push("}\n");
    }
    
    _decl_invoke0 = (count) => {
        var templ;
        
        declval = (count) => {
            return _.range(count).map( (value) => {
                return sprintf("std::declval<%s>()",argType(value));
            }).join(",");
        };

        var values = {
            invokableArgCount: count - 1, 
            templateArgLine: templateArgLine(count),
            templateDeclLine: templateDeclLine(count),
            declval: declval(count)
        }        
                
        templ = `
{{{templateDeclLine}}}
typename std::enable_if<is_invokable{{{invokableArgCount}}}<{{{templateArgLine}}}>::value{{{is_readable}}}, decltype(invoke({{{declval}}}))>::type
inline decl_invoke0();
`
         content = content.concat(Mustache.render(templ, values).split("\n"));
    
        content.push(templateDeclLine(count));        
        var returnType = "typename std::enable_if<!is_invokable{{{invokableArgCount}}}<{{{templateArgLine}}}>::value{{{not_is_readable}}}, Undefined>::type";
        
        content.push(Mustache.render(returnType, values));
        content.push(sprintf("inline decl_invoke0();\n"));

    };
    
    _.range(count).forEach((value) => {
        _invoke(count, value+1);
    });
    _invoke_null(count);
    _is_invokable(count);
    _decl_invoke0(count);
}

content.push("/* BEGIN_GENERATED_CODE */");
_.range(6).forEach( (item) => {
    create_function_set(item+1);
});
content.push("/* END_GENERATED_CODE */");

content = content.map(item => "        " + item);

var source = resolve(__dirname, "../src/cpp/underline.h");
var text = shell.cat(source).toString()

var lines = text.split("\n").reduce((acc, line) => {
    if (line.indexOf("__GENERATOR__") > 0) {
        acc = acc.concat(content);   
    } else {
        acc.push(line);
    }
    return acc;
},[]);

shell.ShellString(lines.join("\n")).to(source);

