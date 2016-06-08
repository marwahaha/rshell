ls > input

cat < input | tr A-Z a-z | tee newOutput1 | tr a-z A-Z > newOutput2

touch output
touch output2

echo a > output
echo b >> output

cat output

echo > c output

cat output

ls > output2
ls | tee output2

cat < input | tee newout
cat newout
