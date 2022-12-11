# $1 -> nome do algorimito que vou testar: threads | serial-opt
# $2 -> quantidade de testes por thread
# $3 -> nbody
# $4 -> nsteps

# $1 $2 $3 $4

#-------------- Melhorando leitura de variaveis------------------
algorithm=$1

[[ $2 = "" ]] && qtdTests=1 || qtdTests=$2 ;

nbody=$3
[[ $3 = "" ]] && nbody_string="default" || nbody_string=$3 ;


nsteps=$4
[[ $3 = "" ]] && nsteps_string="default" || nsteps_string=$3 ;
#----------------------------------------------------------------

function comporate_logs
{
	DIFF=$(diff $result_folder/nbody-serial-$nbody_string-$nsteps_string.txt $result_folder/nbody-$algorithm-$nbody_string-$nsteps_string.txt)
	if [ "$DIFF" == "" ]; then
		echo "   Analisando Logs: NÃO HOUVE DIFERENÇA" 
	else
		echo $DIFF > $result_folder/diff-$numTest\_serial-opt.txt ;
		echo "   Analisando Logs: HOUVE DIFERENÇA, veja o arquivo: $result_folder/diff-$1\_serial-opt.txt" 
	fi
}



echo ""
if [ "$1" != 'threads' ] && [ "$1" != 'serial-opt' ]; then

	if [ "$1" == '-h' ] || [ "$1" == '--help' ]; then
		echo "|--------------------------------------------------------------|"
		echo "|             >>>>>>>> LISTA DE PARÂMETROS <<<<<<<<            |"
		echo "|--------------------------------------------------------------|"
		echo "| 1º | Algoritmo para teste: \"threads\" ou \"serial-opt\"         |"
		echo "|----|---------------------------------------------------------|"
		echo "| 2º | Quantidade que execução do algoritmo para teste         |"
		echo "|----|---------------------------------------------------------|"
		echo "| 3º | Quantidade de corpos na simulação (Default 15)          |"
		echo "|----|---------------------------------------------------------|"
		echo "| 4º | Quantidade de interações na simulação (Default 30000)   |"
		echo "|--------------------------------------------------------------|"
	
	else
		echo "ERRO! ENTRADA INVALIDA:"
		echo "Execute \"-h\" ou \"--help\" para saber os parâmetros"
	fi

else
	echo "1. Compilando com MAKEFILE"
	make

	result_folder=./resultados
	echo ""
	echo "2. Lipando pasta de LOGS"
	rm $result_folder/*.txt

	echo ""
	echo "3. Definido dados de teste"
	echo " Algoritmo testado: $algorithm"
	echo " Quantidade de repetições: $qtdTests"
	echo " Quantidade de corpos: $nbody_string"
	echo " Quantidade de interações da simulação: $nsteps_string"

	echo ""
	echo "4. INICIANDO TESTES CODIGO >>>>>>>> ORIGINAL <<<<<<<< (Rodará $qtdTests vezes):"
	for numTest in $(eval echo "{1..$qtdTests}") ; do
		echo " Rodando o teste de número $numTest..."
		{ time ./nbody-serial.exe $nbody $nsteps 2> $result_folder/nbody-serial-$nbody_string-$nsteps_string.txt ; } 2>> $result_folder/time_original-$nbody_string-$nsteps_string.txt ;
	done


	if [ "$algorithm" == 'serial-opt' ]; then

		echo ""
		echo "5. INICIANDO TESTES CODIGO >>>>>>>> SERIAL OTMIZADO <<<<<<<< (Rodará $qtdTests vezes):"
		for numTest in $(eval echo "{1..$qtdTests}") ; do
			
			echo " Rodando o teste de número $numTest..."
			{ time ./nbody-$algorithm.exe $nbody $nsteps 2> $result_folder/nbody-$algorithm-$nbody_string-$nsteps_string.txt ; } 2>> $result_folder/time_$algorithm-$nbody_string-$nsteps_string.txt ;
			
			comporate_logs $numTest

		done

	else 

		echo ""
		echo "5. INICIANDO TESTES CODIGO >>>>>>>> THREAD OTMIZADO <<<<<<<< (Rodará $qtdTests vezes em cada thread):"
		for thread in {1..8}; do
			echo " Iniciando testes com $thread thread (s)"
					for numTest in $(eval echo "{1..$qtdTests}") ; do
			
						echo "  Rodando o teste de número $numTest..."
						{ time ./nbody-$algorithm.exe $nbody $nsteps 2> $result_folder/nbody-$algorithm-$nbody_string-$nsteps_string.txt ; } 2>> $result_folder/time_$thread\_$algorithm-$nbody_string-$nsteps_string.txt ;
						
						comporate_logs $numTest

					done
		done

	fi

	echo ""
	echo "6. CALCULANDO MÉDIAS DOS TEMPOS"
	python3 media.py

fi
