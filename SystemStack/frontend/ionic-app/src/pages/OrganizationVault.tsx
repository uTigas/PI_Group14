import { IonButton, IonCard, IonCardContent, IonCardHeader, IonCardSubtitle, IonCardTitle, IonCol, IonContent, IonGrid, IonIcon, IonInput, IonItem, IonList, IonListHeader, IonPage, IonRow, IonText, IonTitle } from "@ionic/react"
import { useParams } from "react-router";
import "../support/General.css";
import { arrowBack, arrowDown, arrowUpOutline, cloud, cloudOutline, download, fileTray, fileTrayOutline, trashBin } from "ionicons/icons";
import UploadComponent from "../components/UploadComponent";
import { useContext, useEffect, useState } from "react";
import ApiWrapper from "../support/APIWrapper";
import { UserContext } from "../App";
import { format } from "date-fns";
import Common from "../support/Common";

const  OrganizationVault: React.FC = () => {
    const { id: vaultId }  = useParams<{ id: string }>();
    const [items, setItems] = useState<any[]>([]);
    const [organizations, setOrganizations] = useState<any[]>([]);
    const [members, setMembers] = useState<any[]>([]);

    const userDetails = useContext(UserContext);
    const fetchOrganizationVaultDetails = async () => {
        try {
            const response = await ApiWrapper.fetchOrganizationVaultDetails(vaultId);
            if (response){
                setItems(response.data.items)
                setOrganizations(response.data.organizations)
                setMembers(response.data.members)

            }
        } catch (error) {
            
        }
    }

    const downloadFile = async (id: string, name: string, type: string, ) => {
        try {
            const response = await ApiWrapper.downloadItem(id)
            const blob = new Blob([response!.data], { type: response!.headers['content-type'] });
            
            const url = window.URL.createObjectURL(blob);
            
            const link = document.createElement('a');
            link.href = url;
            link.setAttribute('download', `${name}.${type}`); 
            document.body.appendChild(link);
            link.click();
            
            window.URL.revokeObjectURL(url);
            document.body.removeChild(link);
            
        } catch (error) {
            console.error('Error downloading file:', error);
        }
    }

    useEffect(() => {
        fetchOrganizationVaultDetails()
    },[])

    return(
        <IonPage>
            <IonContent className="ion-padding">
                <IonGrid  className='grid'>
                    <IonRow className="ion-padding-vertical">
                        <IonCol>
                            <IonButton onClick={() => {history.back()}} shape="round"><IonIcon icon={arrowBack} slot="start"></IonIcon></IonButton>
                        </IonCol>
                    </IonRow>
                    <IonRow>
                        <IonCol>

                        </IonCol>
                    </IonRow>
                    <IonRow>
                        <IonCol>
                            <IonTitle>Members</IonTitle>
                            {members.map((member) => (
                                <IonCard key={member.username}>
                                    <IonCardHeader>
                                        <IonCardTitle>{member.name}</IonCardTitle>
                                        <IonCardSubtitle>{member.permissions.map((perm:string) => (<IonText key={perm} color={perm == Common.PERMISSIONS.VIEW ?  ("tertiary"):(perm == Common.PERMISSIONS.EDIT ? ("danger"):("success"))}>{perm}   </IonText>))}</IonCardSubtitle>
                                    </IonCardHeader>
                                    <IonCardContent>
                                    </IonCardContent>
                                </IonCard>
                            ))}
                        </IonCol>
                        <IonCol>
                            <IonTitle>Saved Items</IonTitle>
                            <IonCard>
                                <IonCardContent>
                                    <IonList>
                                        <UploadComponent vaultId={vaultId} user={userDetails?.username}/>                                       
                                    </IonList>
                                </IonCardContent>
                            </IonCard>
                            {items.map((item) => (
                                <IonCard key={item.id}>
                                    <IonCardHeader>
                                        <IonCardTitle>{item.name}</IonCardTitle>
                                        <IonCardSubtitle>Last Change {item.author.fullName} at {format(item.createdAt, "dd-MM-yyyy HH:MM")} </IonCardSubtitle>
                                    </IonCardHeader>
                                    <IonCardContent>
                                        <IonText>{item.size/1000}kb</IonText>
                                        <IonText className="ion-margin-start" >File Format: {item.type}</IonText>
                                        <IonItem>
                                            <IonButton slot="end" color={"tertiary"} className="ion-margin-start" onClick={() => downloadFile(item.id, item.name, item.type)}>Download<IonIcon icon={download}></IonIcon></IonButton>
                                            <IonButton slot="end" color={"danger"} className="ion-margin-start"><IonIcon icon={trashBin}></IonIcon></IonButton>
                                        </IonItem>
                                        
                                    </IonCardContent>
                                </IonCard>
                            ))}
                        </IonCol>
                        <IonCol>
                            <IonTitle>Organizations</IonTitle>
                            {organizations.map((organization) => (
                                <IonCard key={organization.id}>
                                    <IonCardHeader>
                                        <IonCardTitle>{organization.name}</IonCardTitle>
                                        <IonCardSubtitle>{organization.description}</IonCardSubtitle>
                                    </IonCardHeader>
                                    <IonCardContent>
                                    </IonCardContent>
                                </IonCard>
                            ))}
                        </IonCol>
                    </IonRow>
                </IonGrid>
            </IonContent>
        </IonPage>
    )
}
export default OrganizationVault;