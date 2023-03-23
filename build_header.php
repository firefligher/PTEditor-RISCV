#!/usr/bin/env php
<?php

function process_file(&$dst, $work_dir, $file_path) {
  $real_file_path = realpath($work_dir.DIRECTORY_SEPARATOR.$file_path);
  $real_file_dir = dirname($real_file_path);

  $content = @file_get_contents($real_file_path);

  if ($content === FALSE) {
    fprintf(stderr, "Cannot read file at '%s'.\n", $real_file_path);
    return -1;
  }

  // Remove any #pragma once

  $dst = preg_replace("/#pragma\s+once/", "", $content);

  // Resolve includes that use quotation marks as path delimiters (aka the
  // local ones).

  $error = 0;
  $dst = preg_replace_callback(
    "/#include \"(.*?)\"/",
    function ($matches) use (&$error, $real_file_dir, &$success) {
      $error += process_file($content, $real_file_dir, $matches[1]);
      return $content;
    },
    $dst
  );

  // Add some protection to avoid double definitions - #pragma once does not
  // work, if we copy everything to one file...

  $fenceId = "_THE_".md5($real_file_path)."_FENCE_";
  $dst =  "#ifndef $fenceId".PHP_EOL.
          "#define $fenceId".PHP_EOL.
          $dst.PHP_EOL.
          "#endif".PHP_EOL;

  return $error ? -1 : 0;
}

if ($argc != 2) {
  printf("Usage: php %s [input.{c,h}]\n", $argv[0]);
  exit(-1);
}

$status = process_file($content, getcwd(), $argv[1]);

echo($content);
exit($status);
