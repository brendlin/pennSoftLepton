for jet in 0 1
do
	#for sample in 205128 205129 205130 205131 205048 205049 205050 205051 205052 205053 205054 205055 205056
	#for sample in 205128 205129 205130 205131
	for sample in 205128
	do
		./TMVAClassification ${jet} ${sample} Cuts
	done
done
