char ** l2=0; // non-builtin
int l2numitems=0;// font
void freel2(void)
{
    if (!l2)return;
    printf("freeinG font\n");
    int t;
    for (t=0; t<l2numitems; t++)
    {
        free(l2[t]);
    }
    free(l2);
    l2numitems=0;
    l2=0;
}

void loadl2(char * fln)
{
    if(!fln)return;
    FILE * fp = fopen(fln,"r");
    if (fp == NULL)
    {
        printf("no go, cant load font from '%s'\n",fln);
        return;
    }
    freel2();
    int l2size=100;
    size_t s=sizeof(char *) * 100;
    l2=malloc(s);
    if (!l2)
    {
        printf("no place 4 %i byttrez\n",(int)s);
        return;
    }
    size_t len;
    l2numitems=0;
    while ((l2[l2numitems]=fgetln(fp, &len)))
    {
//        printf("%i",len);
        if (!len)printf ("_oooops_\n");
        l2[l2numitems][len-1]=0; // hrrrm turning newline into 0
        l2numitems++;
        if (!(l2numitems%100))
        {
            char **tmp=realloc(l2,sizeof(char *) *(l2size+100));
            if (!tmp)
                return;
            l2=tmp;
            l2size+=100;
        }
    }
    fclose(fp);
}

static char * sgns[] =
{
    "dzdptuzpznxnxpzp", /*\0*/
    "ogooaokkktao",/*\n*/
    "oaaooz",/*<*/
    "azzoaazo",/*>*/
    "akzk    aszs",/*=*/
    "aazzooazzaoooaozooaozo",/***/
    "@llss          owaF",/*;*/
    "saohnoossz",	/*(*/
    "jaohqoosjz",	/*)*/
    "gzwa",	/*/*/
    "luaF",/*,10*/
    "aooooaozoozo",	/*+*/
    "dowo",/*-*/
    "dxxx",	/*_*/
    "mwmzqzqwmw",/*.   14*/
    "aoiasoza",/*~*/
    "iiidqdqiii  ivqvqqiqiv",/*:*/
//    "aooazohohttttr"/*?*/,
    "dawawhohol  oossowlsoo",
    "",/* 18*/
    "gawz",/*\*/
    "oaoz"/*|*/,
    "oaozoozo",/*|-*/
    "hovnowho",/*v22*/
    "oaoz  hsovtvihofti",/*$*/
    "oaaoozzooa  ohos",/*<|>*/
    "-hovnowho"/*v*/
};


char *nums[]=
{
    "@aazz        ",
    "aooaoz",
    "ahoaazzz",
    "aaoaajoraz",
    "aooaaozoooohoz",
    "azzzzoaoaaza",
    "azzzzoaoazaaza",
    "aazaaznmamzm",
    "azzzzoaoazaazazo",
    "azzzzoaoaazazo",
};

static char * chrz[] =
{
    /*A*/"azoazz"
    /*B*/,"aaazzsaoziaa"
    /*C*/,"azaazaaaazzz"
    /*D*/,"azzmaaaz"
    /*E*/,"azaazaaaamzmamazzz"
    /*F*/,"azaazaaaamzmamaz"
    /*G*/,"mommzmzzazaaza"
    /*H*/,"azaaamzmzazz"
    /*I*/,"mzmamz"
    /*J*/,"itmzqtqa"
    /*K*/,"azaaamzaamzz"
    /*L*/,"aaazzz"
    /*M*/,"azaammzazz"
    /*N*/,"azaazzza"
    /*O*/,"azaazazzaz"
    /*P*/,"azaazazmam"
    /*Q*/,"zzazaazazzFF"
    /*R*/,"azaazaamzz"
    /*S*/,"zaaaamzmzzaz"
    /*T*/,"aazamamz"
    /*U*/,"aaazzzza"
    /*V*/,"aamzza"
    /*W*/,"aaizmmrzza"
    /*X*/,"aazzooazza"
    /*Y*/,"aammmzmmza"
    /*Z*/,"aazaazzz"
};
