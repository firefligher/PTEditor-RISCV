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

  $error = 0;
  $dst = preg_replace_callback(
    "/#include \"(.*?)\"/",
    function ($matches) use (&$error, $real_file_dir, &$success) {
      $error += process_file($content, $real_file_dir, $matches[1]);
      return $content;
    },
    $content
  );

  return $error ? -1 : 0;
}

if ($argc != 2) {
  printf("Usage: php %s [input.{c,h}]\n", $argv[0]);
  exit(-1);
}

$status = process_file($content, getcwd(), $argv[1]);

echo($content);
exit($status);
