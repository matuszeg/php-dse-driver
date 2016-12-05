<?php

function logWarning($message) {
    fwrite(STDERR, "Warning: $message\n");
}

function writeDocYaml($yamlFileName, $overwrite, $class) {
    $classDoc = array();
    $classDoc["comment"] = "";
    $methods = $class->getMethods();
    if ($methods) {
        $methodsDoc = array();
        foreach($methods as $method) {
            $methodDoc = array();
            $methodDoc["comment"] = "";
            $parameters = $method->getParameters();
            if ($parameters) {
                $paramsDoc = array();
                foreach ($parameters as $parameter) {
                    $paramDoc = array();
                    $paramDoc["comment"] = "";
                    $parameterType = $parameter->getType();
                    $paramDoc["type"] = $parameterType ? "$parameterType" : "mixed";
                    $paramsDoc[$parameter->getName()] = $paramDoc;
                }
                $methodDoc["params"] = $paramsDoc;
            }
            if (!$method->isConstructor() && !$method->isDestructor()) {
                $returnDoc = array();
                $returnDoc["comment"] = "";
                $returnDoc["type"] = "mixed";
                $methodDoc["return"] = $returnDoc;
            }
            $methodsDoc[$method->getShortName()] = $methodDoc;
        }
        $classDoc["methods"] = $methodsDoc;
    }

    if (!$overwrite && file_exists($yamlFileName)) {
        logWarning("$yamlFileName already exists. Not overwriting.");
        return;
    }

    yaml_emit_file($yamlFileName, array($class->getName() => $classDoc));
}

function usage() {
    die("Usage: {$argv[0]} [-y] -d <directory> OR {$argv[0]} [-y] <className>\n");
}

if (count($argv) < 2) {
    usage();
}

$overwrite = false;

for ($i = 1; $i < count($argv); $i++) {
    $arg = $argv[$i];
    switch($arg) {
    case "-y":
        $overwrite = true;
        break;
    case "-d":
        if ($i + 1 >= count($argv)) {
            die("'-d' requires directory argument\n");
        }
        $directory = $argv[$i + 1];
        $i++;
        break;
    case "-o":
        if ($i + 1 >= count($argv)) {
            die("'-o' requires directory argument\n");
        }
        $yamlFileName = $argv[$i + 1];
        $i++;
        break;
    default:
        $fullClassName = $arg;
        break;
    }
}

if ($directory) {
    define("BASEDIR", $directory);
    define("SRCDIR", BASEDIR . "/src");

    $iterator = new RecursiveIteratorIterator(new RecursiveDirectoryIterator(SRCDIR));
    $regex = new RegexIterator($iterator, '/^.+\.c$/i', RecursiveRegexIterator::GET_MATCH);

    foreach ($regex as $fileName => $notused) {
        $yamlFileName = preg_replace("/(.+)\.c$/", "$1.yaml", $fileName);
        $fileName = substr($fileName, strlen(SRCDIR));
        $fileName = preg_replace("/(.+)\.c$/", "$1", $fileName);

        $fullClassName = str_replace("/", "\\", $fileName);

        try {
            $class = new ReflectionClass($fullClassName);

            echo "Generating doc yaml file for '$fullClassName' ($yamlFileName)\n";

            writeDocYaml($yamlFileName, $overwrite, $class);
        } catch(ReflectionException $e) {
            logWarning("Ignoring '$fullClassName': $e");
        }
    }
} else if ($fullClassName) {
    try {
        $class = new ReflectionClass($fullClassName);

        if (!$yamlFileName) {
            $className = $class->getShortName();
            $yamlFileName = "$className.yaml";
        }

        echo "Generating doc yaml file for '$fullClassName' ($yamlFileName)\n";

        writeDocYaml($yamlFileName, $overwrite, $class);
    } catch(ReflectionException $e) {
        logWarning("Unable to reflect class '$fullClassName': $e");
    }
} else {
    usage();
}
