echo compilando make file
make

result_folder=./resultados

function tempo
{
        { time ./nbody-serial.exe $1 $2 2> $result_folder/nbody-serial-$1-$2.txt ; } 2>> $result_folder/time_serial-$1-$2-test-thread-$3.txt ;
        { time ./nbody-threads.exe $1 $2 $3 2> $result_folder/nbody-threads-$1-$2.txt ; } 2>> $result_folder/time_parallel-$1-$2-test-thread-$3.txt ;
	echo Diferencas encontradas entre as saidas finais:
        diff $result_folder/nbody-serial-$1-$2.txt $result_folder/nbody-threads-$1-$2.txt
}

function teste
{
	rm $result_folder/*.txt
	for threads in {1..8}; do
		echo iniciando testes para threads = $threads
		for teste in $(eval echo "{1..$3}") ; do
			echo teste numero $teste  
			tempo $1 $2 $threads
		done
	done
	echo "calculando médias"
	python3 media.py
}

# $1 -> nome da funcao
# $2 -> nbody
# $3 -> nsteps
# $4 -> quantidade de testes por thread

$1 $2 $3 $4
