USERNAME="complete with your username"
PASSWORD="complete with your password"
zip -u cpp_starter_package.zip
curl -d "username=$USERNAME&password=$PASSWORD" http://ai-contest.com/check_login.php --cookie cookies.txt --cookie-jar cookies.txt
curl --form uploadedfile=@cpp_starter_package.zip --form MAX_FILE_SIZE=2000000 http://ai-contest.com/check_submit.php --cookie cookies.txt --cookie-jar cookies.txt
