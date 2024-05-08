import { IonButton, IonCard, IonCardContent, IonCardHeader, IonCardTitle, IonCol, IonContent, IonGrid, IonIcon, IonPage, IonRouterOutlet, IonRow, IonText, IonTitle } from "@ionic/react";
import ApiWrapper from "../components/APIWrapper";
import { Route, useParams } from "react-router-dom";
import { useEffect, useState } from "react";
import { arrowBack, backspace, returnDownBack, warning } from "ionicons/icons";
import CreateVaultContainer from "../components/CreateVaultContainer";
import "./General.css"
import AddMemberContainer from "../components/AddMemberContainer";

const Organization: React.FC = () => {
    const { id: organizationId }  = useParams<{ id: string }>();
    const [vaults, setVaults] = useState<any[]>([]);
    const [members, setMembers] = useState<any[]>([]);
    const [role, setRole] = useState<string>("");

    useEffect(() => {
        fetchOrganization();
    }, [])

    const fetchOrganization = async() => {
        try{
        const response = await ApiWrapper.fetchOrganizationDetails(organizationId);
        if (response){
            setMembers(response.data.members)
            setVaults(response.data.vaults)
            setRole(response.data.role)

        }
        else{
            setVaults([])
            setMembers([])
            setRole("")

        }
        } catch (error){
        console.error('Error fetching Organization Details', error);
        }
    }

    const handleGoBack = () => {
        history.back(); 
      };

    return (
        <IonPage>
            <IonContent className="ion-padding">
                <IonGrid className='grid'>
                    <IonRow>
                        <IonCol>
                            <IonIcon onClick={() => handleGoBack()} icon={arrowBack} size="large" color="tertiary" ></IonIcon>
                        </IonCol>
                        <IonCol  className="ion-text-right ion-padding-end">
                            <IonText>Your Role: <IonText color="warning">{role}</IonText></IonText>
                        </IonCol>
                    </IonRow>
                    <IonRow className="ion-margin-vertical">
                        <IonCol size="4">
                            <IonTitle className='title'>Create Vault:</IonTitle>
                            <IonCard>
                                <IonCardContent className="ion-margin-bottom ">
                                    <CreateVaultContainer/>
                                </IonCardContent>
                            </IonCard>
                            <IonTitle className='ion-margin-top title'>Add Member:</IonTitle>
                            <IonCard>
                                <IonCardContent>
                                    <AddMemberContainer/>
                                </IonCardContent>
                            </IonCard>

                        </IonCol>
                        <IonCol size="4">
                            <IonTitle className='ion-text-start  title'>Members:</IonTitle>
                            {members.length !== 0 ? (
                                members.map((item, index) => (
                                <IonCard className="card  organization-card" key={item.username}>
                                    <IonCardHeader>
                                        <IonCardTitle>{item.name}</IonCardTitle>
                                    </IonCardHeader>
                                    <IonCardContent>
                                        <strong>{item.role}</strong>
                                    </IonCardContent>
                                </IonCard>
                                ))
                            ):(
                                <p>No members found.</p>
                            )} 
                        </IonCol>
                        <IonCol>
                            
                            <IonTitle className=' title'>Vaults:</IonTitle>
                            {vaults.length !== 0 ? (
                                vaults.map((item, index) => (
                                <IonCard className="card  organization-card" key={item.id}>
                                    <IonCardHeader>
                                        <IonCardTitle>{item.name}</IonCardTitle>
                                    </IonCardHeader>
                                    <IonCardContent>
                                        <strong>{item.description}</strong>
                                    </IonCardContent>
                                </IonCard>
                                ))
                            ):(
                                <strong className='ion-margin-start'>No vaults yet.</strong>
                            )} 
                        </IonCol>
                    </IonRow>
                </IonGrid>
            </IonContent>
        </IonPage>
    );
};

export default Organization;