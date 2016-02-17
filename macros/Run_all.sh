for zerojet in true false
do
	for lower in true false
	do
		for logy in true
		do
			for cuts in "(MinMll<25)"
			do
				root -q -b 'drawSignif.C("jet0Pt", 50, 0., 400., "#it{p}_{T}^{jet}", "GeV", false, '${logy}', '${lower}',true, "'${cuts}'")'
				root -q -b 'drawSignif.C("lep0Pt", 40, 0., 160., "#it{p}_{T}^{lead}", "GeV", '${zerojet}', '${logy}', '${lower}',true, "'${cuts}'")'
				root -q -b 'drawSignif.C("lep1Pt", 40, 0., 160., "#it{p}_{T}^{sub}", "GeV", '${zerojet}', '${logy}', '${lower}',true, "'${cuts}'")'
				root -q -b 'drawSignif.C("lep2Pt", 40, 0., 160., "#it{p}_{T}^{third}", "GeV", '${zerojet}', '${logy}', '${lower}',true, "'${cuts}'")'
				root -q -b 'drawSignif.C("met_Et", 40, 0., 200., "#it{E}_{T}^{miss}", "GeV", '${zerojet}', '${logy}', '${lower}',true, "'${cuts}'")'
				root -q -b 'drawSignif.C("MinMll", 50, 0., 200., "#it{m}_{ll}^{min}", "GeV", '${zerojet}', '${logy}', '${lower}',true, "'${cuts}'")'
				root -q -b 'drawSignif.C("Mll", 50, 0., 200., "#it{m}_{ll}", "GeV", '${zerojet}', '${logy}', '${lower}',true, "'${cuts}'")'
				root -q -b 'drawSignif.C("MTWmin", 50, 0., 200., "#it{m}_{T}^{W}", "GeV", '${zerojet}', '${logy}', '${lower}',true, "'${cuts}'")'
				root -q -b 'drawSignif.C("DeltaPhiMETJet", 30, 0., 3.1415, "#Delta#phi(#it{E}_{T}^{miss},jet)", "rad", false, '${logy}', '${lower}',true, "'${cuts}'")'
				root -q -b 'drawSignif.C("DeltaPhi3LMET", 30, 0., 3.1415, "#Delta#phi(#it{E}_{T}^{miss},3l)", "rad", '${zerojet}', '${logy}', '${lower}',true, "'${cuts}'")'
				root -q -b 'drawSignif.C("M3l", 50, 0., 300., "#it{m}_{3l}", "GeV", '${zerojet}', '${logy}', '${lower}',true, "'${cuts}'")'
				root -q -b 'drawSignif.C("(lep0Pt/jet0Pt)", 60, 0., 3., "#it{p}_{T}^{lead}/#it{p}_{T}^{jet}", "", false, '${logy}', '${lower}',true, "'${cuts}'")'
				root -q -b 'drawSignif.C("Pt3l", 40, 0., 160., "#it{p}_{T}^{3l}", "GeV", '${zerojet}', '${logy}', '${lower}',true, "'${cuts}'")'
				root -q -b 'drawSignif.C("Ptll", 40, 0., 160., "#it{p}_{T}^{ll(leads)}", "GeV", '${zerojet}', '${logy}', '${lower}',true, "'${cuts}'")'
			done
		done
	done
done
