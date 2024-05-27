import { IonAlert, IonButton, IonCard, IonCardContent, IonCardHeader, IonCardSubtitle, IonCardTitle, IonCol, IonContent, IonGrid, IonHeader, IonIcon, IonInput, IonItem, IonItemDivider, IonLabel, IonList, IonListHeader, IonPage, IonPopover, IonRow, IonSearchbar, IonText, IonTitle } from "@ionic/react"
import { useParams } from "react-router";
import "../support/General.css";
import { addOutline, arrowBack, arrowDown, arrowUpOutline, cloud, cloudOutline, createOutline, download, fileTray, fileTrayOutline, trashBin, trashBinOutline } from "ionicons/icons";
import UploadComponent from "../components/UploadComponent";
import { useContext, useEffect, useState } from "react";
import ApiWrapper from "../support/APIWrapper";
import { UserContext } from "../App";
import { format } from "date-fns";
import Common from "../support/Common";
import AppAppBar from "../components/AppAppBar";

const  OrganizationVault: React.FC = () => {
    const { id: vaultId }  = useParams<{ id: string }>();
    const [items, setItems] = useState<any[]>([]);
    const [organizations, setOrganizations] = useState<any[]>([]);
    const [members, setMembers] = useState<any[]>([]);
    const [title, setTitle] = useState<string>("");
    const [fileName, setFileName] = useState<string>("");

    const userDetails = useContext(UserContext);
    const fetchOrganizationVaultDetails = async () => {
        try {
            const response = await ApiWrapper.fetchOrganizationVaultDetails(vaultId);
            if (response){
                console.log(response);
                setOrganizations(response.data.organizations)
                setMembers(response.data.members)
                setItems(response.data.items);
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

    const deleteItem = (id: string) => {
        const formData = new FormData;
        formData.append("id", id);
        ApiWrapper.deleteItem(formData)
        fetchOrganizationVaultDetails();
    }

    const renameItem = async (id: string) => {
        try {

            const formData = new FormData;
            formData.append("id", id);
            formData.append("name", fileName);
            const response = await ApiWrapper.renameItem(formData);
            fetchOrganizationVaultDetails();

            if (response) {
                setFileName('')
            }
            else {
                setFileName('')
            }
        } catch (error) {
            console.error('Error renaming file', error);
        }
    }

    useEffect(() => {
        fetchOrganizationVaultDetails();
    },[])

    return(
        <IonPage>
            <IonHeader>
                <AppAppBar title={"Organization/Vault"} />
            </IonHeader>
            <IonContent>
                <IonGrid>
                    <IonRow>
                        <IonCol size="auto">
                            <IonButton className="return_button" onClick={() => history.back()} fill="outline" size="small" color={"secondary"}>
                                <IonIcon icon={arrowBack} size="large"></IonIcon>
                                Return
                            </IonButton>
                        </IonCol>
                    </IonRow>
                    <IonRow>
                        <IonCol>
                            <IonCard>
                                <IonCardContent>
                                    <IonGrid>
                                        <IonTitle>Items</IonTitle>
                                        <IonRow>
                                            <IonCol>
                                                <IonSearchbar mode="ios" animated={true} color='' placeholder='Search for Items...'></IonSearchbar>
                                            </IonCol>
                                            <IonCol size="auto">
                                                <IonButton className="create-org" color={'success'} shape="round" fill='outline' id="click-trigger">Add<IonIcon icon={addOutline} /></IonButton>
                                                <IonPopover trigger="click-trigger" triggerAction="click">
                                                    <UploadComponent vaultId={vaultId} user={""} />
                                                </IonPopover>
                                            </IonCol>

                                        </IonRow>
                                        <IonRow>
                                            <IonCol><IonLabel><h2>Name</h2></IonLabel></IonCol>
                                            <IonCol><IonLabel><h2>Size</h2></IonLabel></IonCol>
                                            <IonCol><IonLabel><h2>Type</h2></IonLabel></IonCol>
                                            <IonCol><IonLabel><h2>Creation</h2></IonLabel></IonCol>
                                            <IonCol></IonCol>
                                        </IonRow>
                                        <IonItemDivider/>
                                        {items.length !== 0 ? (
                                            items.map((item) => (
                                                <div key={item.id}>
                                                    <IonRow>
                                                        <IonCol className='appt_col'>
                                                            <IonLabel>{item.name}</IonLabel>
                                                        </IonCol>
                                                        <IonCol className='appt_col'>
                                                            <IonLabel>{item.size / 1000 / 1024 > 1000 ? (item.size / 1000 / 1024 / 1024).toFixed(2) + " GB" : item.size / 1000 > 1024 ? (item.size / 1000 / 1024).toFixed(2) + " MB" : (item.size / 1000).toFixed(2) + " KB"} </IonLabel>
                                                        </IonCol>
                                                        <IonCol className='appt_col'>
                                                            <IonLabel>{item.type}</IonLabel>
                                                        </IonCol>
                                                        <IonCol className='appt_col'>
                                                            <IonLabel>{format(item.createdAt, "dd-MM-yyyy HH:MM")}</IonLabel>
                                                        </IonCol>
                                                        <IonCol>
                                                            <div className="appt_button">
                                                                <IonButton id={"delete-" + item.id} shape='round' fill='outline' color={'danger'} size='small'><IonIcon size="medium" icon={trashBinOutline} /></IonButton>
                                                                <IonButton id={"rename-" + item.id} shape='round' fill='outline' color={'success'} size='small'><IonIcon size="medium" icon={createOutline} /></IonButton>
                                                                <IonAlert
                                                                    trigger={"delete-" + item.id}
                                                                    trigger-action="click"
                                                                    header="Delete File"
                                                                    subHeader="This is irreversible!"
                                                                    message="Do you wish to confirm file deletion?"
                                                                    buttons={[
                                                                        {
                                                                            text: 'YES',
                                                                            handler: () => {
                                                                                deleteItem(item.id);
                                                                            }
                                                                        },
                                                                        {
                                                                            text: 'NO',
                                                                            role: 'cancel',
                                                                            handler: () => {
                                                                            }
                                                                        },
                                                                    ]}
                                                                ></IonAlert>
                                                                <IonPopover
                                                                    trigger={"rename-" + item.id}
                                                                    trigger-action="click"

                                                                >
                                                                    <IonItem>
                                                                        <IonInput placeholder='Insert file name' onIonChange={(e) => { if (e.detail.value) setFileName(e.detail.value) }}></IonInput>
                                                                    </IonItem>
                                                                    <IonItem>
                                                                        <IonButton color={'success'} fill='outline' onClick={() => { if (fileName != '') renameItem(item.id) }}>Rename</IonButton>
                                                                    </IonItem>
                                                                </IonPopover>
                                                            </div>

                                                        </IonCol>
                                                    </IonRow>
                                                    <IonItemDivider>
                                                    </IonItemDivider>
                                                </div>
                                            ))
                                        ) : (
                                            <IonText>No Items stored yet. Start uploading! </IonText>
                                        )}
                                    </IonGrid>
                                </IonCardContent>
                            </IonCard>
                        </IonCol>
                    </IonRow>
                </IonGrid>
            </IonContent>
        </IonPage>
    )
}
export default OrganizationVault;