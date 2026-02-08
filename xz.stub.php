<?php

/**
 * @generate-legacy-arginfo
 * @generate-class-entries
 * @undocumentable
 */

/** @alias fread */
function xzread(resource $fp, ?int $length = UNKNOWN): string|false {}

/** @alias fwrite */
function xzwrite(resource $fp, string $str, ?int $length = UNKNOWN): int|false {}

/** @alias fclose */
function xzclose(resource $fp): bool {}

/** @alias fpassthru */
function xzpassthru(resource $fp): int|false {}

function xzencode(string $str, ?int $compression_level = null): string|false {}

function xzdecode(string $str): string|false {}

function xzopen(string $filename, string $mode, int $compression_level = UNKNOWN): resource|false {}