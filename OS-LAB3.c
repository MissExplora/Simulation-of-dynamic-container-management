//
//  main.c
//  Labos
//
//  Created by Dora Budić on 1/10/13.
//  Copyright (c) 2013 Dora Budić. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>

typedef struct node{
    int pocetak;
    int velicina;
    int oznaka;
    struct node *sljed_svi;
    struct node *sljed_slob;
    struct node *sljed_zauz;
}node;


node *svi, *zauzeti, *slobodni;


int br_blokova = 1;
int br_slobodnih = 1;
int br_zauzetih = 0;



void *NadjiSlobodni (int velicina)
{
    // vraca prvog clana slobodne liste u koji stane novi blok
    
    node *pom;
    
    pom = slobodni;
    
    if (pom->sljed_slob == NULL)
    {
        printf("Nema slobodnih blokova!\n");
        exit(-1);
    }
    
    else
    {
        do
        {
            pom = pom->sljed_slob;
            if (pom == NULL)
            {
                printf("Ne postoji dovoljno veliki slobodni blok!\n");
                exit(-1);
            }
        } while (pom->velicina < velicina);
    }
    return pom;
    
}




void MakniIzSlobodnih()
{
    // mice novonastali zauzeti blok iz liste slobodnih
    
    node *prije, *sad;
    
    prije = slobodni;
    sad = slobodni->sljed_slob;
    
    if (sad == NULL)
    {
        printf("Nema slobodnih blokova!\n");
        exit(1);
    }
    
    else
    {
        while (sad->oznaka == 0)
        {
            prije = sad;
            sad = sad->sljed_slob;
        }
    }
    
    prije->sljed_slob = sad->sljed_slob;
    sad->sljed_slob = NULL;
    
}




void NadjiMjesto(node *cvor, int velicina)
{
    // trazi mjesto u slobodnoj listi za zadani cvor tako da se ocuva sortiranost po velicini bloka od najmanjeg prema najvecem
    
    node *prije, *sad;
    
    prije = slobodni;
    sad = prije->sljed_slob;
    
    if (sad == NULL)
    {
        slobodni->sljed_slob = cvor;
        cvor->sljed_slob = NULL;
    }
    
    else
    {
        while (sad->velicina < velicina)
        {
            if (sad->pocetak > cvor->pocetak)
            {
                cvor->sljed_slob = sad->sljed_slob;
                sad->sljed_slob = cvor;
                return;
            }
            prije = sad;
            sad = sad->sljed_slob;
            if (sad == NULL)
                break;
            
        }
        cvor->sljed_slob = sad;
        prije->sljed_slob = cvor;
        
    }
}




void NaKrajZauzetih(node *cvor)
{
    // sprema novi zauzeti cvor na kraj liste zauzetih blokova
    
    node *pom;
    
    pom = zauzeti;
    
    if (pom->sljed_zauz != NULL)
    {
        while (pom->sljed_zauz != NULL)
            pom = pom->sljed_zauz;
    }
    cvor->sljed_zauz = NULL;
    pom->sljed_zauz = cvor;
}




void *dodijeli (int velicina)
{
    // stvara zauzeti blok iz slobodnog (u slucaju da bi podjela na 2 dijela dala premali slobodan blok) ili jednog slobodnog dijeli na zauzeti i slobodni
    
    node *stari;
    node *novi;
    node *pom;
    
    stari = NadjiSlobodni (velicina);
    
    if ((stari->velicina - velicina) > 4)
    {
        // dijelim blok na jedan slobodan i jedan zauzet
        
        novi = (node *) malloc (sizeof(node));
        novi->velicina = stari->velicina - velicina;
        novi->pocetak = stari->pocetak + velicina;
        novi->oznaka = 0;
        novi->sljed_svi = stari->sljed_svi;
        novi->sljed_zauz = NULL;
        pom = stari->sljed_slob;
        
        stari->velicina = velicina;
        stari->oznaka = 1;
        stari->sljed_svi = novi;
        stari->sljed_slob = novi;
        MakniIzSlobodnih();
        NaKrajZauzetih(stari);
        novi->sljed_slob = pom;
        br_zauzetih++;
        br_blokova++;
    }
    
    else
    {
        stari->oznaka = 1;
        MakniIzSlobodnih();
        NaKrajZauzetih(stari);
        br_zauzetih++;
        br_slobodnih--;
    }
    
    return stari;
}




void RazmjestiSlobodnu(node *cvor)
{
    // razmjesta listu slobodnih s obzirom na udruzivanje susjednih slobodnih blokova
    
    node *prije, *sad;
    
    prije = slobodni;
    sad = prije->sljed_slob;
    
    if (sad == NULL)
    {
        sad = cvor;
        cvor->sljed_slob = NULL;
    }
    
    else
    {
        while (sad->sljed_slob != NULL)
        {
            if (sad->velicina > sad->sljed_slob->velicina)
            {
                if (sad->sljed_slob->sljed_slob == NULL)
                {
                    sad->sljed_slob = NULL;
                    return;
                }
                else
                    break;
            }
            
            prije = sad;
            sad = sad->sljed_slob;
        }
        
        prije->sljed_slob = sad->sljed_slob;
        NadjiMjesto(cvor, cvor->velicina);
    }
    
}




void RazmjestiZauzetu(node *cvor)
{
    // mice blok iz liste zauzetih
    
    node *prije, *sad;
    
    prije = zauzeti;
    sad = prije->sljed_zauz;
    
    while(sad != NULL)
    {
        if (sad->pocetak == cvor->pocetak)
            break;
        prije = sad;
        sad = sad->sljed_zauz;
    }
    prije->sljed_zauz = sad->sljed_zauz;
    sad->sljed_zauz = NULL;
}




void oslobodi(int adr_programa)
{
    // oslobadja blok na zadanoj adresi s obzirom na polozaj u listi svih blokova
    
    node *prije, *sad;
    int zastavica = 0;
    
    prije = svi;
    sad = svi->sljed_svi;
    
    
    while (sad->pocetak != adr_programa)
    {
        if (sad->sljed_svi == NULL)
        {
            printf("Ne postoji zauzet blok sa odabranom pocetnom adresom!\n");
            exit(-1);
        }
        prije = sad;
        sad = sad->sljed_svi;
    }

    if (sad->oznaka == 0)
    {
        printf("Ovaj blok je vec slobodan!\n");
        exit(-1);
    }
    
    if (sad->sljed_svi == NULL)
    {
        if (prije->oznaka == 0)
        {
            prije->velicina += sad->velicina;
            prije->sljed_svi = NULL;
            RazmjestiSlobodnu(prije);
            RazmjestiZauzetu(sad);
            free(sad);
            br_blokova--;
            br_zauzetih--;
        }
        
        else
        {
            sad->oznaka = 0;
            sad->sljed_zauz= NULL;
            NadjiMjesto(sad, sad->velicina);
            RazmjestiZauzetu(sad);
            br_zauzetih--;
            br_slobodnih++;
        }
    }
    
    else if (sad->pocetak == 0)
    {
        prije = sad;
        sad = sad->sljed_svi;
        
        if (sad->oznaka == 0)
        {
            prije->velicina += sad->velicina;
            prije->sljed_svi = sad->sljed_svi;
            RazmjestiZauzetu(prije);
            RazmjestiSlobodnu(prije);
            free(sad);
            br_zauzetih--;
            br_blokova--;
        }
        
        else
        {
            prije->oznaka = 0;
            NadjiMjesto(prije, prije->velicina);
            RazmjestiZauzetu(prije);
            br_zauzetih--;
            br_slobodnih++;
        }
    }
    
    else if (prije->oznaka == 1 && sad->sljed_svi->oznaka == 1)
    {
        // tip A
        
        sad->oznaka = 0;
        prije->sljed_zauz = sad->sljed_zauz;
        sad->sljed_zauz = NULL;
        NadjiMjesto(sad, sad->velicina);
        br_zauzetih--;
        br_slobodnih++;
    }
    
    else if (prije->oznaka == 1 && sad->sljed_svi->oznaka == 0)
    {
        // tip B
        
        prije->sljed_svi = sad->sljed_svi;
        sad->sljed_svi->velicina += sad->velicina;
        sad->sljed_svi->pocetak = sad->pocetak;
        prije->sljed_zauz = sad->sljed_zauz;
        RazmjestiSlobodnu(sad->sljed_svi);
        free(sad);
        br_zauzetih--;
        br_blokova--;
    }
    
    else if(prije->oznaka == 0 && sad->sljed_svi->oznaka == 1)
    {
        // tip C
        
        prije->sljed_svi = sad->sljed_svi;
        prije->velicina += sad->velicina;
        RazmjestiZauzetu(sad);
        RazmjestiSlobodnu(prije);
        free(sad);
        br_zauzetih--;
        br_blokova--;
    }
    
    else
    {
        // tip D
        
        prije->velicina = prije->velicina + sad->velicina + sad->sljed_svi->velicina;
        if (sad->sljed_svi->sljed_svi != NULL)
            prije->sljed_svi = sad->sljed_svi->sljed_svi;
        else
            zastavica++;
        RazmjestiZauzetu(sad); 
        RazmjestiSlobodnu(prije);
        free(sad->sljed_svi);
        free(sad);
        if (zastavica != 0)
            prije->sljed_svi = NULL;
        br_zauzetih--;
        br_slobodnih--;
        br_blokova -= 2;
    }
}




int main()
{
    int zahtjev;
    int velicina;
    int adr_programa;
    int i;
    
    node *cvor, *pom;
    
    svi = (node *) malloc (sizeof(node));    
    zauzeti = (node *) malloc (sizeof(node));
    slobodni = (node *) malloc (sizeof(node));

    cvor = (node *) malloc (sizeof(node));
    cvor->pocetak = 0;
    cvor->velicina = 10000;
    cvor->oznaka = 0;
    cvor->sljed_slob = NULL;
    cvor->sljed_svi = NULL;
    cvor->sljed_zauz = NULL;
    
    slobodni->sljed_slob = cvor;
    svi->sljed_svi = cvor;
    zauzeti->sljed_zauz = NULL;
    
    
    
    while (1)
    {
        i = 0;
        pom = svi;
        
        
        printf("\nRS: broj blokova = %d, slobodni = %d, zauzeti = %d\n", br_blokova, br_slobodnih, br_zauzetih);
        
        
        while (pom->sljed_svi != NULL)
        {
            pom = pom->sljed_svi;
            printf("%d: pocetak = %d, velicina = %d, oznaka = %d\n", i, pom->pocetak, pom->velicina, pom->oznaka);
            i++;
        }
        
        
        printf("\nUnesi zahtjev (1-dodjeli, 0-oslobodi): ");
        scanf("%d", &zahtjev);
        
        
        if (zahtjev == 1)
        {
            // dodijeli
            
            printf("\nUnesi velicinu programa (u oktetima): ");
            scanf("%d", &velicina);
            if (velicina <= 0 || velicina > 10000)
            {
                printf("Neispravno zadana velicina!\n");
                exit(-1);
            }
            pom = dodijeli (velicina+12);
            printf("\nDodijeljen blok na adresi %d\n", pom->pocetak);
        }
        
        else if (zahtjev == 0)
        {
            // oslobodi
            
            printf("\nUnesi pocetnu adresu programa: ");
			scanf("%d", &adr_programa);
			oslobodi(adr_programa);
        }
        
        else
        {
            // krivo
            
            printf("Neispravno zadana operacija!\n");
            return -1;
        }
    }
    
    
    
    return 0;
}



