cd cpp_starter_package
make clean
make all
#java -jar tools/PlayGame.jar maps/map72.txt 1000 1000 log.txt "java -jar example_bots/DualBot.jar" "./MyBot"    | java -jar tools/ShowGame.jar
#java -jar tools/PlayGame.jar maps/map12.txt 1000 1000 log.txt "./MyBot001" "./MyBot"    | java -jar tools/ShowGame.jar
#java -jar tools/PlayGame.jar maps/map92.txt 1000 1000 log.txt "./MyBot001" "./MyBot"    | java -jar tools/ShowGame.jar
#java -jar tools/PlayGame.jar maps/map96.txt 1000 1000 log.txt "./MyBot001" "./MyBot"    | java -jar tools/ShowGame.jar
#java -jar tools/PlayGame.jar maps/map11.txt 1000 1000 log.txt "./MyBot001" "./MyBot"    | java -jar tools/ShowGame.jar
#java -jar tools/PlayGame.jar extra_maps/game-5902469.txt 1000 1000 log.txt "java -jar example_bots/RageBot.jar" "./MyBot"    | java -jar tools/ShowGame.jar
java -jar tools/PlayGame.jar maps/map1.txt 1000 1000 log.txt "java -jar example_bots/RageBot.jar" "./MyBot"    | java -jar tools/ShowGame.jar
cd ..
