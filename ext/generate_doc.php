<?php

define("LICENSE_COMMENT", "/*
  Copyright (c) 2016 DataStax, Inc.
  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/");

define("DOC_COMMENT_HEADER", "/**" . PHP_EOL);
define("DOC_COMMENT_LINE", " * ");
define("DOC_COMMENT_FOOTER", " */" . PHP_EOL);

define("PHP_SYNTAX_CHECK", "php -l -n");

if (count($argv) < 2) {
    die("Usage: {$argv[0]} <directory>" . PHP_EOL);
}

define("BASEDIR", $argv[1]);
define("SRCDIR", BASEDIR . "/src");
define("DOCDIR", BASEDIR . "/doc");
define("INDENT", "    ");

function startsWith($haystack, $needle) {
    $length = strlen($needle);
    return (substr($haystack, 0, $length) === $needle);
}

function isAlreadyImplementedByBase($current, $implemented) {
    $parentClass = $current->getParentClass();
    if ($parentClass && $parentClass != $implemented){
        if ($parentClass->implementsInterface($implemented) ||
            $parentClass->isSubclassOf($implemented)) {
                return true;
        }
    }

    $interfaces = $current->getInterfaces();
    foreach($interfaces as $interface) {
        if ($interface != $implemented) {
            if ($interface->implementsInterface($implemented) ||
                $interface->isSubclassOf($implemented)) {
                    return true;
                }
        }
    }

    return false;
}

function logWarning($message) {
    fwrite(STDERR, "Warning: $message" . PHP_EOL);
}

function writeCommentDoc($file, $comment, $indent = 0) {
    $lines = explode(PHP_EOL, $comment);
    if (strlen(end($lines)) == 0) {
        array_pop($lines);
    }
    foreach($lines as $line) {
        $commentLine = str_pad("", strlen(INDENT) * $indent, INDENT, STR_PAD_LEFT) . DOC_COMMENT_LINE . "$line";
        $commentLine = rtrim($commentLine) . PHP_EOL;
        fwrite($file, str_pad($commentLine, $indent, INDENT, STR_PAD_LEFT));
    }
}

function writeParameterDoc($doc, $file, $class, $method, $parameter) {
    $className = $class->getName();
    $methodName = $method->getShortName();
    $parameterName = $parameter->getName();
    $parameterType = $parameter->getType();

    if (isset($doc[$className]['methods'][$methodName]['params'][$parameterName])) {
        $parameterDoc = $doc[$className]['methods'][$methodName]['params'][$parameterName];
        if (isset($parameterDoc['comment'])) {
            $comment = trim($parameterDoc['comment']);
        } else {
            logWarning("Missing '$parameterName' parameter's 'comment' documentation for method '$className.$methodName'");
        }
        if (isset($parameterDoc['type'])) {
            $type = trim($parameterDoc['type']);
        } else {
            logWarning("Missing '$parameterName' parameter's 'type' documentation for method '$className.$methodName'");
        }

        $parameterType = $parameterType ? $parameterType : "mixed";
        $parameterType = $type ? $type : $parameterType; # Overrides builtin if provided
        $commentLine = "@param $parameterType \$$parameterName $comment";
        $commentLine = rtrim($commentLine) . PHP_EOL;
        fwrite($file, INDENT . DOC_COMMENT_LINE . $commentLine);
    } else {
        $parameterType = $parameterType ? $parameterType : "mixed";
        fwrite($file, INDENT . DOC_COMMENT_LINE . "@param $parameterType \$$parameterName" . PHP_EOL);
        logWarning("Missing parameter '$parameterName' documentation for method '$className.$methodName'");
    }
}

function writeReturnDoc($doc, $file, $class, $method) {
    if ($method->isConstructor() || $method->isDestructor()) return;

    $className = $class->getName();
    $methodName = $method->getShortName();

    if (isset($doc[$className]['methods'][$methodName]['return'])) {
        $returnDoc = $doc[$className]['methods'][$methodName]['return'];
        if (isset($returnDoc['comment'])) {
            $comment = trim($returnDoc['comment']);
        } else {
            logWarning("Missing return 'comment' documentation for method '$className.$methodName'");
        }
        if (isset($returnDoc['type'])) {
            $type = trim($returnDoc['type']);
        } else {
            logWarning("Missing return 'type' documentation for method '$className.$methodName'");
        }

        $type = $type ? $type : "mixed";
        $commentLine = "@return $type $comment";
        $commentLine = rtrim($commentLine) . PHP_EOL;
        fwrite($file, INDENT . DOC_COMMENT_LINE . $commentLine);
    } else {
        fwrite($file, INDENT . DOC_COMMENT_LINE . "@return mixed" . PHP_EOL);
        logWarning("Missing 'return' documentation for method '$className.$methodName'");
    }
}

function writeMethodDoc($doc, $file, $class, $method) {
    $className = $class->getName();
    $methodName = $method->getShortName();

    fwrite($file, INDENT . DOC_COMMENT_HEADER);
    if (isset($doc[$className]['methods'][$methodName])) {
        $methodDoc = $doc[$className]['methods'][$methodName];
        if (isset($methodDoc['comment'])) {
            $comment = $methodDoc['comment'];
        } else {
            logWarning("Missing 'comment' documentation for method '$className.$methodName'");
        }
        if (count($method->getParameters()) > 0 && !isset($methodDoc['params'])) {
            logWarning("Missing 'params' documentation for method '$className.$methodName'");
        }

        $methodName = $method->getShortName();
        $parameters = $method->getParameters();

        if ($comment) {
            if (count($parameters) > 0) {
                $comment .= PHP_EOL;
            }
            writeCommentDoc($file, $comment, 1);
        }
        foreach ($parameters as $parameter) {
            writeParameterDoc($doc, $file, $class, $method, $parameter);
        }
        writeReturnDoc($doc, $file, $class, $method);
    } else {
        fwrite($file, INDENT . DOC_COMMENT_LINE . "@return mixed" . PHP_EOL);
        logWarning("Missing documentation for method '$className.$methodName'");
    }

    fwrite($file, INDENT . DOC_COMMENT_FOOTER);
}

function writeMethod($doc, $file, $class, $method) {
    writeMethodDoc($doc, $file, $class, $method);

    if ($method->isPrivate()) {
        fwrite($file, INDENT . "private ");
    } else if ($method->isProtected()) {
        fwrite($file, INDENT . "protected ");
    } else if ($method->isPublic()) {
        fwrite($file, INDENT . "public ");
    }

    if ($method->isStatic()) {
        fwrite($file, "static ");
    }

    if ($method->isFinal()) {
        fwrite($file, "final ");
    }

    if (!$class->isInterface() && $method->isAbstract()) {
        fwrite($file, "abstract ");
    }

    $methodName = $method->getShortName();
    fwrite($file, "function $methodName(");

    $parameters = $method->getParameters();
    $first = true;
    foreach ($parameters as $parameter) {
        if (!$first) fwrite($file, ", ");
        $parameterName = $parameter->getName();
        fwrite($file, "\$$parameterName");
        $first = false;
    }

    if ($class->isInterface() || $method->isAbstract()) {
        fwrite($file, ");" . PHP_EOL);
    } else {
        fwrite($file, ") { }" . PHP_EOL);
    }

    fwrite($file, PHP_EOL);
}

function writeClassDoc($doc, $file, $class) {
    $className = $class->getName();

    fwrite($file, DOC_COMMENT_HEADER);
    if (isset($doc[$className])) {
        if (isset($doc[$className]['comment'])) {
            writeCommentDoc($file, $doc[$className]['comment']);
        } else {
            logWarning("Missing 'comment' documentation for class '$className'");
        }
        if (count($class->getMethods()) > 0 && !isset($doc[$className]['methods'])) {
            logWarning("Missing 'methods' documentation for class '$className'");
        }
    } else {
        fwrite($file, DOC_COMMENT_LINE . PHP_EOL);
        logWarning("Missing documentation for class '$className'");
    }
    fwrite($file, DOC_COMMENT_FOOTER);
}

function writeClass($doc, $file, $class) {
    $namespace = $class->getNamespaceName();
    $className = $class->getShortName();

    writeClassDoc($doc, $file, $class);

    if ($class->isInterface()) {
        fwrite($file, "interface ");
    } else if ($class->isAbstract()) {
        fwrite($file, "abstract class ");
    } else if ($class->isFinal()) {
        fwrite($file, "final class ");
    } else {
        fwrite($file, "class ");
    }

    fwrite($file, "$className ");

    $parentClass = $class->getParentClass();
    if ($parentClass) {
        $parentClassName = $parentClass->getName();
        if (startsWith($parentClassName, $namespace)) {
            $parentClassName = $parentClass->getShortName();
        }
        fwrite($file, "extends $parentClassName ");
    }

    $interfaces = $class->getInterfaces();
    if ($interfaces) {
        $first = true;
        foreach ($interfaces as $interface) {
            if (isAlreadyImplementedByBase($class, $interface)) {
                continue;
            }
            $interfaceName = $interface->getName();
            if (startsWith($interfaceName, $namespace)) {
                $interfaceName = $interface->getShortName();
            }
            if ($first) {
                fwrite($file, "implements ");
            } else {
                fwrite($file, ", ");
            }
            fwrite($file, "$interfaceName");
            $first = false;
        }
        fwrite($file, " ");
    }

    fwrite($file, "{" . PHP_EOL);
    fwrite($file, PHP_EOL);

    $methods = $class->getMethods();
    if ($methods) {
        foreach($methods as $method) {
            writeMethod($doc, $file, $class, $method);
        }
    }

    fwrite($file, "}" . PHP_EOL);
}

$iterator = new RecursiveIteratorIterator(new RecursiveDirectoryIterator(SRCDIR));
$regex = new RegexIterator($iterator, '/^.+\.c$/i', RecursiveRegexIterator::GET_MATCH);

foreach ($regex as $fileName => $notused) {
    $yamlFileName = preg_replace("/(.+)\.c$/", "$1.yaml", $fileName);
    $fileName = substr($fileName, strlen(SRCDIR));
    $fileName = preg_replace("/(.+)\.c$/", "$1", $fileName);

    $fullClassName = str_replace("/", "\\", $fileName);

    try {
        $namespaceDirectory = DOCDIR . dirname($fileName);

        if (!is_dir($namespaceDirectory)) {
            if (!mkdir($namespaceDirectory, 0777, true)) {
                die("Unable to create directory '$namespaceDirectory'" . PHP_EOL);
            }
        }

        $class = new ReflectionClass($fullClassName);

        $className = $class->getShortName();

        $stubFileName = "$namespaceDirectory/$className.php";
        echo "Generating stub for '$fullClassName' ($stubFileName)" . PHP_EOL;
        if(!($file = fopen($stubFileName, "w"))) {
            die("Unable to create file '$stubFileName'" . PHP_EOL);
        }

        fwrite($file, "<?php" . PHP_EOL);
        fwrite($file, PHP_EOL);
        fwrite($file, LICENSE_COMMENT . PHP_EOL);
        fwrite($file, PHP_EOL);

        $namespace = $class->getNamespaceName();
        if ($namespace) {
            fwrite($file, "namespace $namespace;" . PHP_EOL);
            fwrite($file, PHP_EOL);
        }

        $doc = yaml_parse_file($yamlFileName);

        if ($doc === false) {
            logWarning("Unable to open doc yaml file '$yamlFileName'");
        }

        writeClass($doc, $file, $class);

        fclose($file);

        exec(PHP_SYNTAX_CHECK . " $stubFileName", $syntaxCheckOutput, $syntaxCheckReturnVar);
        if ($syntaxCheckReturnVar !== 0) {
            $syntaxCheckOutput = implode(PHP_EOL, $syntaxCheckOutput);
            die("Syntax invalid for '$fullClassName' ($stubFileName): $syntaxCheckOutput" . PHP_EOL);
        }
    } catch(ReflectionException $e) {
        logWarning("Ignoring '$fullClassName': $e");
    }
}
