/****************************************
 ****************** DATAS ***************
 ***************************************/

/***************** AA ***********************/
REGISTER_AND_CONSTRUCT(AA,"website_aa",slug,"slug",mass,"mass");

/***************** Enzyme *************************/
REGISTER_AND_CONSTRUCT(Enzyme,"website_enzyme",name,"name");

/**************** ANALYSEMGF *******************/
//REGISTER_AND_CONSTRUCT(AnalyseMgf,"website_analysemgf",mgf,"mgf");
M2M_REGISTER(AnalyseMgf,AAs,AA,"website_analysemgf_AAs","analysemgf_id","aa_id")
REGISTER(AnalyseMgf,"website_analysemgf",mgf,"mgf",enzyme,"enzyme_id")
AnalyseMgf::AnalyseMgf() : mgf(AnalyseMgf::_mgf), enzyme(AnalyseMgf::_enzyme), AAs(*this)
{
    mgf.registerAttr(*this);
    enzyme.registerAttr(*this);
}


/********************** AnalysePeptide ******************************/
REGISTER_AND_CONSTRUCT(AnalysePeptide,"website_analysepeptide",analyse,"analyse_id",name,"name",mz,"mz",intensity,"intensity",charge,"charge",mgf_part,"mgf_part",status,"status");


/******************** CalculatedPeptide *****************************/
REGISTER_AND_CONSTRUCT(CalculatedPeptide,"website_calculatedpeptide",score,"score",sequence,"sequence",analyse,"analyse_id");

/**********************************************************
 ******************** COMMUNICATION ***********************
 *********************************************************/

/************** Server ***************/
REGISTER_AND_CONSTRUCT(HarpeServer,"website_harpeserver",name,"name",ip,"ip",port,"port",is_active,"is_active");


/*************** Client ***************/
REGISTER_AND_CONSTRUCT(Client,"website_client",ip,"ip",port,"port",server,"server_id",is_active,"is_active");

/*************** M2M Client->AnalysePeptide *************/
REGISTER_AND_CONSTRUCT(ClientCalculation,"website_clientcalculation",client,"client_id",analysepeptide,"analysepeptide_id",status,"status");

