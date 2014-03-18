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

    //AAModificationPosition
    std::list<orm::Cache<AAModificationPosition>::type_ptr> aamodificationposition;
    AAModificationPosition::query()\
        .filter(self.pk,"exact",AAModificationPosition::_modification)\
        .get(aamodificationposition);

    stream<<self.pk
        <<self.delta
        <<(unsigned int)aamodificationposition.size();

    for (auto& aamod : aamodificationposition)
        stream<<*aamod;

    return stream;
}

/***************** AA Modification Position *********************/
REGISTER_AND_CONSTRUCT(AAModificationPosition,"website_aamodificationposition",aa,"AA_id",modification,"modification_id",position,"position");

ntw::Serializer& operator<<(ntw::Serializer& stream,AAModificationPosition& self)
{
    stream<<self.pk
        <<*self.aa
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
    stream<<self.pk;
    // AAs
    {
        auto aas = self.AAs.all();
        stream<<(unsigned int)aas.size();
        for(auto& aa_ptr: aas)
            stream<<*aa_ptr;
    }

    //modifications PTMs
    {
        auto modifications = self.modifications.all();
        stream<<(unsigned int)modifications.size();

        for(auto& mod : modifications)
            stream<<*mod;
    }

    return stream;
}


/********************** AnalysePeptide ******************************/
REGISTER_AND_CONSTRUCT(AnalysePeptide,"website_analysepeptide",analyse,"analyse_id",name,"name",mz,"mz",mass,"mass",intensity,"intensity",charge,"charge",mgf_part,"mgf_part",status,"status");

ntw::Serializer& operator<<(ntw::Serializer& stream,AnalysePeptide& self)
{
    stream<<self.pk
        <<self.mgf_part
        <<*(self.analyse);
    return stream;
}

/******************** CalculatedPeptide *****************************/
REGISTER_AND_CONSTRUCT(CalculatedPeptide,"website_calculatedpeptide",score,"score",\
                       sequence,"sequence",\
                       analyse,"analyse_id",\
                       error_total,"error_total",\
                       error_aa_cumul,"error_aa_cumul",\
                       intensitee_total_parent,"intensitee_total_parent",\
                       intensitee_total,"intensitee_total",
                       mass_total,"mass_total",\
                       percent_couverture,"percent_couverture",\
                       nb_aa,"nb_aa",\
                       nb_peaks,"nb_peaks",\
                       mass_parent,"mass_parent",\
                       percent_intensitee_utilisee,"percent_intensitee_utilisee");

/**********************************************************
 ******************** COMMUNICATION ***********************
 *********************************************************/

/************** Server ***************/
REGISTER_AND_CONSTRUCT(HarpeServer,"communication_harpeserver",name,"name",ip,"ip",port,"port",is_active,"is_active");


/*************** Client ***************/
REGISTER_AND_CONSTRUCT(Client,"communication_client",ip,"ip",port,"port",server,"server_id",is_active,"is_active");

/*************** M2M Client->AnalysePeptide *************/
REGISTER_AND_CONSTRUCT(ClientCalculation,"communication_clientcalculation",client,"client_id",analysepeptide,"analysepeptide_id",status,"status");

