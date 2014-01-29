/****************************************
 ****************** DATAS ***************
 ***************************************/

/***************** AA ***********************/
REGISTER_AND_CONSTRUCT(AA,"website_aa",slug,"slug",mass,"mass");


ntw::Serializer& operator<<(ntw::Serializer& stream,const AA& self)
{
    stream<<self.pk
        <<self.slug
        <<self.mass;
    return stream;
}

/*************** AA Modification ***********************************/
REGISTER_AND_CONSTRUCT(AAModification,"website_aamodification",name,"name",delta,"delta");

ntw::Serializer& operator<<(ntw::Serializer& stream,const AAModification& self)
{
    stream<<self.pk
        <<self.delta;
    return stream;
}

/***************** AA Modification Position *********************/
REGISTER_AND_CONSTRUCT(AAModificationPosition,"website_aamodificationposition",aa,"AA_id",modification,"modification_id",position,"position");

ntw::Serializer& operator<<(ntw::Serializer& stream,const AAModificationPosition& self)
{
    stream<<self.pk
        //<<self.AA
        //<<self.modification
        <<self.position;
    return stream;
}


/***************** Enzyme *************************/
REGISTER_AND_CONSTRUCT(Enzyme,"website_enzyme",name,"name");

/**************** ANALYSEMGF *******************/
//REGISTER_AND_CONSTRUCT(AnalyseMgf,"website_analysemgf",mgf,"mgf");
M2M_REGISTER(AnalyseMgf,AAs,AA,"website_analysemgf_AAs","analysemgf_id","aa_id")
M2M_REGISTER(AnalyseMgf,modification,AAModification,"website_analysemgf_modifications","analysemgf_id","aamodification_id")
REGISTER(AnalyseMgf,"website_analysemgf",mgf,"mgf",enzyme,"enzyme_id")
AnalyseMgf::AnalyseMgf() : mgf(AnalyseMgf::_mgf), enzyme(AnalyseMgf::_enzyme), AAs(*this), modifications(*this)
{
    mgf.registerAttr(*this);
    enzyme.registerAttr(*this);
}

ntw::Serializer& operator<<(ntw::Serializer& stream,const AnalyseMgf& self)
{
    auto aas = self.AAs.all(*AA::default_connection);
    stream<<self.pk
        <<(unsigned int)aas.size();
    for(auto& aa_ptr: aas)
        stream<<*aa_ptr;
    return stream;
}


/********************** AnalysePeptide ******************************/
REGISTER_AND_CONSTRUCT(AnalysePeptide,"website_analysepeptide",analyse,"analyse_id",name,"name",mz,"mz",intensity,"intensity",charge,"charge",mgf_part,"mgf_part",status,"status");

ntw::Serializer& operator<<(ntw::Serializer& stream,AnalysePeptide& self)
{
    stream<<self.pk
        <<self.mgf_part
        <<*(self.analyse);
    return stream;
}

/******************** CalculatedPeptide *****************************/
REGISTER_AND_CONSTRUCT(CalculatedPeptide,"website_calculatedpeptide",score,"score",sequence,"sequence",analyse,"analyse_id");

/**********************************************************
 ******************** COMMUNICATION ***********************
 *********************************************************/

/************** Server ***************/
REGISTER_AND_CONSTRUCT(HarpeServer,"communication_harpeserver",name,"name",ip,"ip",port,"port",is_active,"is_active");


/*************** Client ***************/
REGISTER_AND_CONSTRUCT(Client,"communication_client",ip,"ip",port,"port",server,"server_id",is_active,"is_active");

/*************** M2M Client->AnalysePeptide *************/
REGISTER_AND_CONSTRUCT(ClientCalculation,"communication_clientcalculation",client,"client_id",analysepeptide,"analysepeptide_id",status,"status");

