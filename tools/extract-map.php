<?php
$game_id = $_SERVER["argv"][1];
if(empty($game_id)) die("uso: \n php ". $_SERVER["argv"][0]. " [game-id]\n\n");
$url = "http://www.ai-contest.com/visualizer.php?game_id=" .$game_id;
$content = file_get_contents($url);
preg_match("/playback_string=([^\|]+)\|/", $content, $matches);
#preg_match("/playback_string/", $content, $matches);

if(empty($matches) or empty($matches[1])) die('No se pudo encontrar la partida en '.$url."\n");
$planet = preg_split("/:/", $matches[1]);
foreach($planet as $info){
  echo "P ". preg_replace("|,|", " ", $info) . "\n";;
}
